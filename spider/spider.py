#本文件需要安装python 3 和newspaper库。以及数据库mysql数据库

#抓取的新闻网站：
#联合早报：需要屏蔽掉订户https://www.zaobao.com.sg/znews/greater-china
#bbc中文网 https://www.bbc.com/zhongwen/simp/chinese_news

import newspaper
import re
import pymysql
import os
import urllib 
import requests
from newspaper import Article

def connectDatabase():
    db = pymysql.connect(host="localhost",# your host, usually localhost
                         user="root",         # your username
                         passwd="news/123",  # your password
                         db="news",        # name of the data base
                         autocommit=True, use_unicode=True, charset="utf8")
    cur = db.cursor()
    return cur    

def closDatabase(cur):
    cur.close()

def checkUrl(cur,url):
    cur.execute("SELECT id FROM newstable WHERE url = %s" ,url)
    row = cur.fetchone();
    return row

def writeDatabase(cur, url):
        article = Article(url,language = 'zh')
        article.download()
        article.parse()
        print(article.title)
        print("\n")
        print(article.top_image)
        print("\n")
        print(len(article.text))
        print("\n")
        print(article.publish_date)
        print("\n")
         
        #需要去除短小无效内容以及重复的url,
        #下载图片并重新命名为
        if len(article.text) < 500 or checkUrl(cur,url):
            return
        else:
            cur.execute(
           """INSERT INTO  newstable(title,article, url)
              VALUES (%s, %s, %s)""",
              (article.title, article.text,article.url))
            image = checkUrl(cur,url)
            #print(image)
            #print("\n")
            uploadServer(article.top_image,image[0])
def bbcSpider(cur):
    allurls = newspaper.build('https://www.bbc.com/zhongwen/simp/chinese_news')
    bbc = re.findall(r"(?<=href=\")\/zhongwen\/simp\/chinese-news-.*?(?=\")", allurls.html)
    #需要保证逆序列，保证最新的在在最后
    for link in bbc[::-1]:
        #print("https://www.bbc.com"+link)
        writeDatabase(cur,"https://www.bbc.com"+link)   
          
def zaobaoSpider(cur):
    allurls = newspaper.build('https://www.zaobao.com.sg/znews/greater-china')
    zaobao = re.findall(r"(?<=href=\")\/znews\/greater-china\/.*?(?=\")", allurls.html)
    for link in zaobao[::-1]:
        #print("https://www.zaobao.com.sg"+link)
        writeDatabase(cur, "https://www.zaobao.com.sg"+link)  
def uploadServer(url,image):
        os.chdir('/root/news') #打开要保存的文件夹
        urllib.request.urlretrieve(url,'%s.jpg' %image)
        os.system('scp  '+str(image) +'.jpg root@47.92.96.91:/root/news')
def main():
    cur = connectDatabase()
    zaobaoSpider(cur)
    bbcSpider(cur)
    closDatabase(cur)
main()




