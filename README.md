# KINGSOFT WPS Office LPE
***WPS Office*** is an office suite for Microsoft Windows, macOS, Linux, iOS, Android, and HarmonyOS developed by Zhuhai-based Chinese software developer Kingsoft.

## Exploring WPS 
One of the nicest features that WPS offers is a cloud service to save your documents , work etc ... , this service by default is set to manual , it's started only if you navigate to WPS cloud into the WPS office panel but the service gets started with the current user privilege (Low priv). 
## Vulnerability 
Looking into the early imports done by wps cloud service once started , it looks like it will first try to import a DLL called  **CRYPTSP.DLL** and other ones from ***C:\ProgramData\kingsoft\office6\*** if they aren't there and by default they aren't , the service will load it from System32 as you can see : <br/><br/>
<img src="/assets/process_monitor.PNG"/><br/><br/>
The issue here is that the ACL for that directory is configured as read write to all users, an attacker can plant a malicious DLL there and restart the executable , but it gets started as current priv level (low priv user) , unless we start the executable as service (Since it's installed as one) with something like **net start wpscloudsvr** which will start the service as **NT AUTHORITY** . <br/>
The issue here seems to be more of an ***ACL misconfiguration*** .
## Exploit
My exploit is simple , it will copy the crafted DLL (  change Administrator password ) to the target directory restart the service , now an access to administrator account is available , which means I have access to sedebugpriv from there I steal the winlogon token and start cmd as **NT AUTHORITY / System** . <br/>
## PoC 





https://user-images.githubusercontent.com/57273771/152659158-7f3a5607-40d9-41b6-85c5-7ed3ca83d0e5.mp4

