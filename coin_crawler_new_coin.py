import os
from selenium import webdriver
from selenium.webdriver.common.by import By
import time

chrome_options = webdriver.ChromeOptions()
chrome_options.add_argument('--headless')
chrome_options.add_argument('--no-sandbox')
chrome_options.add_argument('--single-process')
chrome_options.add_argument('--disable-dev-shm-usage') 

URL = 'https://www.coingecko.com/ko/new-cryptocurrencies'

driver = webdriver.Chrome('/home/sinsa/wrtn/chromedriver', options=chrome_options)
driver.get(URL)

#depth_1_tbody = driver.find_element(By.XPATH, "//tbody[@data-target='currencies.contentBox']")
depth_1_tbody = driver.find_element(By.TAG_NAME,"tbody")
depth_2_rows = depth_1_tbody.find_elements(By.TAG_NAME, "tr")

#print(depth_1_tbody.text)
#print(depth_2_rows[0].text)

coin_explain_list = [['코인', '심볼', '시세', '24시간 등락률', '24시간 거래량', '추가시점']]
coin_list = []

f = open('cryptocurrency_newcoin.txt', 'w', encoding="utf-8")
#print(str(coin_list[0]))
f.write(str(coin_explain_list[0]) + "\n")


#for i in range(0, 10):
#    coin_list.append(depth_2_rows[i].text.split("\n"))
#    f.write(str(coin_list[i+1]) + "\n")
    
for i in range(0, 10):
    depth_3_body = depth_2_rows[i].find_elements(By.TAG_NAME, "td")
    for index, value in enumerate(depth_3_body):
        #print(index, value.text)
        if index == 2 or index == 3 or index == 6 or index == 7 or index == 9:
            if index == 2:
                coin_list.append(value.text.replace("\n", ", "))
            else:
                coin_list.append(value.text)
            #f.write(str(value.text))
        else:
            continue
    #f.write("\n")


#print(coin_list)

result = [coin_list[i:i+5] for i in range(0, len(coin_list), 5)]

#print(result)

for i in range(0, 10):
    f.write(str(result[i]) + "\n")

f.write("\n")
f.write("현재시각 : " + time.strftime('%Y.%m.%d - %H:%M:%S'))

#print(coin_list)
    
f.close()

