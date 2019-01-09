#本文件需要安装python 3 和newspaper库。以及数据库mysql数据库

#抓取的新闻网站：
#联合早报：需要屏蔽掉订户https://www.zaobao.com.sg/znews/greater-china
#bbc中文网 https://www.bbc.com/zhongwen/simp/chinese_news
 #service mysqld stop
 #service mysqld start
 #show tables;
 #CREATE DATABASE news; 
 #use news;
 #CREATE TABLE newstable (id INT UNSIGNED NOT NULL PRIMARY KEY AUTO_INCREMENT,title TINYTEXT,bimg tinyint(1) ,pubdate VARCHAR(10),url VARCHAR(100),article TEXT) ENGINE=InnoDB,default character set utf8;
#每隔30分钟执行一次
# */30 * * * * python /root/news/spider.py
import newspaper
import re
import pymysql
import os
import urllib 
import requests
import datetime
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
    

def writeDatabase(cur, url,filter):
        article = Article(url,language = 'zh')
        article.download()
        article.parse()
        #print(article.title)
        print("\n")
        #print(article.top_image)
        print("\n")
        #print(len(article.text))
        print("\n")
         
        #需要去除短小无效内容以及重复的url,
        if filter and len(article.text) < 500:
            return
        elif checkUrl(cur,url):
            return
        else:
            pubdate = datetime.date.today()
            if(article.top_image.find('.jpg') == -1):
                bimg = 0
                cur.execute("""INSERT INTO  newstable(title,bimg,pubdate, url,article)VALUES (%s, %s, %s,%s, %s)""",
                (article.title,bimg,pubdate,article.url,article.text))
            else:
                bimg = 1
                cur.execute(
                """INSERT INTO  newstable(title,bimg,pubdate, url,article)
                VALUES (%s, %s,%s, %s, %s)""",
                (article.title,bimg,pubdate,article.url,article.text))
                #下载图片并重新命名为
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
        writeDatabase(cur,"https://www.bbc.com"+link,1)   
        
        
        
          
def zaobaoSpider(cur):
    allurls = newspaper.build('https://www.zaobao.com.sg/znews/greater-china')
    zaobao = re.findall(r"(?<=href=\")\/znews\/greater-china\/.*?(?=\")", allurls.html)
    for link in zaobao[::-1]:
        #print("https://www.zaobao.com.sg"+link)
        writeDatabase(cur, "https://www.zaobao.com.sg"+link,1)  
        
def realtimeSpider(cur):
    allurls = newspaper.build('https://www.zaobao.com.sg/realtime/china')
    zaobao  = re.findall(r"(?<=href=\")\/realtime\/china\/.*?(?=\")", allurls.html)
    for link in zaobao[::-1]:
        #print("https://www.zaobao.com.sg"+link)
        writeDatabase(cur, "https://www.zaobao.com.sg"+link,0)      
        
        
        
def uploadServer(url,image):
        os.chdir('/root/news/image') #打开要保存的文件夹
        urllib.request.urlretrieve(url,'%s.jpg' %image)
        os.system('scp  '+str(image) +'.jpg root@47.92.96.91:/root/news/image')
        
        
        
def main():
    cur = connectDatabase()
    realtimeSpider(cur)
    zaobaoSpider(cur)
    bbcSpider(cur)
    closDatabase(cur)
main()




