#ifndef MUDUO_BASE_ASYNCLOGGING_H
#define MUDUO_BASE_ASYNCLOGGING_H

#include <stdio.h>

#include <muduo/base/BlockingQueue.h>
#include <muduo/base/BoundedBlockingQueue.h>
#include <muduo/base/CountDownLatch.h>
#include <muduo/base/Mutex.h>
#include <muduo/base/Thread.h>
#include <muduo/base/LogStream.h>
#include <muduo/base/LogFile.h>
#include <muduo/base/Timestamp.h>

#include <boost/bind.hpp>
#include <boost/noncopyable.hpp>
#include <boost/scoped_ptr.hpp>
#include <boost/ptr_container/ptr_vector.hpp>

namespace muduo
{

	class AsyncLogging : boost::noncopyable
	{
	public:

		AsyncLogging(const string& basename,
			size_t rollSize,
			int flushInterval = 3);

		~AsyncLogging()
		{
			if (running_)
			{
				stop();
			}
		}

		void append(const char* logline, int len);

		void start()
		{
			running_ = true;
			thread_.start();
			latch_.wait();
		}

		void stop()
		{
			running_ = false;
			cond_.notify();
			thread_.join();
		}

	private:

		// declare but not define, prevent compiler-synthesized functions
		AsyncLogging(const AsyncLogging&);  // ptr_container
		void operator=(const AsyncLogging&);  // ptr_container

		void threadFunc();

		typedef muduo::detail::FixedBuffer<muduo::detail::kLargeBuffer> Buffer;
		typedef boost::ptr_vector<Buffer> BufferVector;
		typedef BufferVector::auto_type BufferPtr;

		const int flushInterval_;
		bool running_;
		string basename_;
		size_t rollSize_;
		muduo::Thread thread_;
		muduo::CountDownLatch latch_;
		muduo::MutexLock mutex_;
		muduo::Condition cond_;
		BufferPtr currentBuffer_;
		BufferPtr nextBuffer_;
		BufferVector buffers_;
	};

	AsyncLogging::AsyncLogging(const string& basename,size_t rollSize,int flushInterval)
							: flushInterval_(flushInterval),
							  running_(false),
							  basename_(basename),
							  rollSize_(rollSize),
							  thread_(boost::bind(&AsyncLogging::threadFunc, this), "Logging"),
							  latch_(1),
							  mutex_(),
							  cond_(mutex_),
							  currentBuffer_(new Buffer),
							  nextBuffer_(new Buffer),
							  buffers_()
	{
		currentBuffer_->bzero();
		nextBuffer_->bzero();
		buffers_.reserve(16);
	}

	void AsyncLogging::append(const char* logline, int len)
	{
		muduo::MutexLockGuard lock(mutex_);
		if (currentBuffer_->avail() > len)
		{
			currentBuffer_->append(logline, len);
		}
		else
		{
			buffers_.push_back(currentBuffer_.release());

			if (nextBuffer_)
			{
				currentBuffer_ = boost::ptr_container::move(nextBuffer_);
			}
			else
			{
				currentBuffer_.reset(new Buffer); // Rarely happens
			}
			currentBuffer_->append(logline, len);
			cond_.notify();
		}
	}

	void AsyncLogging::threadFunc()
	{
		assert(running_ == true);
		latch_.countDown();
		LogFile output(basename_, rollSize_, false);
		BufferPtr newBuffer1(new Buffer);
		BufferPtr newBuffer2(new Buffer);
		newBuffer1->bzero();
		newBuffer2->bzero();
		BufferVector buffersToWrite;
		buffersToWrite.reserve(16);
		while (running_)
		{
			assert(newBuffer1 && newBuffer1->length() == 0);
			assert(newBuffer2 && newBuffer2->length() == 0);
			assert(buffersToWrite.empty());

			{
				muduo::MutexLockGuard lock(mutex_);
				if (buffers_.empty())  // unusual usage!
				{
					cond_.waitForSeconds(flushInterval_);
				}
				buffers_.push_back(currentBuffer_.release());
				currentBuffer_ = boost::ptr_container::move(newBuffer1);
				buffersToWrite.swap(buffers_);
				if (!nextBuffer_)
				{
					nextBuffer_ = boost::ptr_container::move(newBuffer2);
				}
			}

			assert(!buffersToWrite.empty());

			if (buffersToWrite.size() > 25)
			{
				char buf[256];
				snprintf(buf, sizeof buf, "Dropped log messages at %s, %zd larger buffers\n",
					Timestamp::now().toFormattedString().c_str(),
					buffersToWrite.size() - 2);
				fputs(buf, stderr);
				output.append(buf, static_cast<int>(strlen(buf)));
				buffersToWrite.erase(buffersToWrite.begin() + 2, buffersToWrite.end());
			}

			for (size_t i = 0; i < buffersToWrite.size(); ++i)
			{
				// FIXME: use unbuffered stdio FILE ? or use ::writev ?
				output.append(buffersToWrite[i].data(), buffersToWrite[i].length());
			}

			if (buffersToWrite.size() > 2)
			{
				// drop non-bzero-ed buffers, avoid trashing
				buffersToWrite.resize(2);
			}

			if (!newBuffer1)
			{
				assert(!buffersToWrite.empty());
				newBuffer1 = buffersToWrite.pop_back();
				newBuffer1->reset();
			}

			if (!newBuffer2)
			{
				assert(!buffersToWrite.empty());
				newBuffer2 = buffersToWrite.pop_back();
				newBuffer2->reset();
			}

			buffersToWrite.clear();
			output.flush();
		}
		output.flush();
	}

}
#endif  // MUDUO_BASE_ASYNCLOGGING_H
