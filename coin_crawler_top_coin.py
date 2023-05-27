import os
from selenium import webdriver
from selenium.webdriver.common.by import By
#from webdriver_manager.chrome import ChromeDriverManager
import time

chrome_options = webdriver.ChromeOptions()
chrome_options.add_argument('--headless')
chrome_options.add_argument('--no-sandbox')
chrome_options.add_argument('--single-process')
chrome_options.add_argument('--disable-dev-shm-usage') 


URL = 'https://www.coingecko.com/ko'

#driver = webdriver.Chrome(ChromeDriverManager().install())
driver = webdriver.Chrome('/home/sinsa/wrtn/chromedriver', options=chrome_options)
driver.get(URL)

#depth_1_tbody = driver.find_element(By.XPATH, "//tbody[@data-target='currencies.contentBox']")
depth_1_tbody = driver.find_element(By.TAG_NAME,"tbody")
depth_2_rows = depth_1_tbody.find_elements(By.TAG_NAME, "tr")
depth_3_body = depth_2_rows[0].find_elements(By.TAG_NAME, "td")

#print(depth_1_tbody.text)
#print(depth_2_rows[0].text)

#for i in range(0, 10):
#    print(depth_3_body[i].text)
#print(depth_3_body[0].text, depth_3_body[5].text)

coin_list = [['순위', '코인', '심볼', '매수(생략가능)', '시세', '24시간 거래량', '시가총액']]

f = open('cryptocurrency_realtime_price.txt', 'w', encoding="utf-8")
#print(str(coin_list[0]))
f.write(str(coin_list[0]) + "\n")


for i in range(0, 10):
    coin_list.append(depth_2_rows[i].text.split("\n"))
    data = coin_list[i+1]
    last_element = data[-1]
    split_elements = last_element.split(' ')
    split_elements_with_commas = [elem.replace(',', '') + '%' if '%' in elem else elem for elem in split_elements]
    
    data = data[:-1] + split_elements_with_commas  
    substring_to_check = "%%"
    #substring_to_check2 = "Buy"

    filtered_data = []

    for element in data:
        if substring_to_check not in element:
            filtered_data.append(element)

    f.write(str(filtered_data) + "\n")
    
f.write("\n")
f.write("현재시각 : " + time.strftime('%Y.%m.%d - %H:%M:%S'))

#print(coin_list)

'''
data = coin_list[1]

last_element = data[-1]  # Get the last element of the list
split_elements = last_element.split(' ')  # Split the last element using space delimiter
split_elements_with_commas = [elem.replace(',', '') + '%' if '%' in elem else elem for elem in split_elements]

data = data[:-1] + split_elements_with_commas  # Concatenate the modified split elements with the rest of the data list

substring_to_check = "%%"

# Create a new list to store the filtered data
filtered_data = []

# Iterate over each element in the data
for element in data:
    # Check if the substring is present in the element
    if substring_to_check not in element:
        # If the substring is not present, add the element to the filtered data
        filtered_data.append(element)

# Print the filtered data
print(filtered_data)
'''

driver.quit()
f.close()

