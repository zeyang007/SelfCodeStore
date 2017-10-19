import MonthLen

Months = "JanuaryFebruaryMarchAprilMayJuneJulyAugustSeptemberOctoberNovemberDecember"

while(True):
    monthStr = input("please input the month: ")

    if(monthStr == "quit"):
        break

    if(not(monthStr in ["1","2","3","4","5","6","7","8","9","10","11","12"])):
        print("invalid input,please try again.")
        continue

    month = int(monthStr)

    headPosition = 0
    endPosition = 0
    count = 1
    
    while(month > 0):
        headPosition = endPosition
        endPosition = endPosition + MonthLen.getMonthLength(count)
        count = count + 1
        month = month - 1

    print(Months[headPosition:endPosition])
