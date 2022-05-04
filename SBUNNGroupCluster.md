## FAQ

```
Q: Ivy shut down due to power outage. After manual machine reboot, logging on gives permission denied error:
Permission denied (publickey,gssapi-keyex,gssapi-with-mic).
```

A: Need to log in ivy as root and manually restart the ```ypbind``` [deamon](https://www.oreilly.com/openbook/linag2/book/ch13.html) which distributes users info (key, password, etc).

```
Q: How many GPUs can I use on the group machine?
```

Each user can at most request 2 GPUs on a single machine (one machine has 2 GPUs).

```
Q: How to find out the amount of space taken up by my home directory in the nnhome machine?
```

A: Go to your home directory ```cd /home/<your usr name>``` and do ```du -hc --max-depth=0 .```.
