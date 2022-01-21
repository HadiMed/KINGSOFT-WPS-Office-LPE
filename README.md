# KINGSOFT WPS Office LPE
**WPS office** is now a competitor of microsoft office , its developed by **Zhuhai Kingsoft**, it has some nice features , good UI and it has a good FREE version , I started using it years ago but never had the idea to reverse it , or check how its working till recently .

## Exploring WPS 
One of the nicest features that WPS offers is a cloud service to save your documents , work etc ... , this service by default is set to manual , it's started only if you navigate to WPS cloud into the WPS office panel but the service gets started with the current user privilege (Low priv). 
## Vulnerability 
Looking into the early imports done by wps cloud service once started , it looks like it will first try to import a DLL called  **CRYPTSP.DLL** and other ones from ***C:\ProgramData\kingsoft\office6\*** if they aren't there and by default they aren't , the service will load it from System32 as you can see : <br/><br/>
<img src="/assets/process_monitor.PNG"/><br/><br/>
by the way that directory is write-able by any user , we can plant a DLL there and restart the service , but it gets started as current user , unless we start the service with a command like **net start wpscloudsvr** which will start the service as **NT AUTHORITY** . <br/>
The issue here seems to be more of an ***ACL misconfiguration*** since not just one DLL is loaded from that library . 
## Exploit
My exploit is simple , it will copy the  crafted DLL ( to change the Administrator password ) to the target directory restart the service , now we have access to administrator account which means I have access to sedebugpriv from there I steal the winlogon token and start cmd as **NT AUTHORITY / System** . <br/>
## PoC 


https://user-images.githubusercontent.com/57273771/125179100-1e63fa00-e1b9-11eb-91f5-aa6bf0378df5.mp4

