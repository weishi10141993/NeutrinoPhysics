## FAQ

Read the login message first!

```
Q: What does this Slurm error mean: Unable to contact slurm controller (connect failure)?
```
A: It usually happens because the Slurm controller deamon isn't active. You can check this by typing ```systemctl status slurmctld``` and check the ```Loaded``` and ```Active``` row output. If they are failed, it usually means the root admin needs to restart the deamon. This happens often after a machine reboot. In general, you can do ```systemctl status <deamon>``` to check any deamon.

```
Q: How to run a batch job from nnhome?
```
A: Say your need to run this program ```root -l -q -b XXX.C```, and it needs to run ~1hr on one node, then use this [batch script](https://github.com/weishi10141993/NeutrinoPhysics/blob/main/SBU/Slurm_nnhome_example.sh) and do ```sbatch Slurm_nnhome_example.sh```

```
Q: Ivy shut down due to power outage. After manual machine reboot, logging in gives permission denied error:
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

```
Q: What to do if lost access to /storage/shared space? (example on ivy)
```
First check the directory is mounted, i.e., check "autofs" deamon. Then if mounted, check if it's due to some job running out of memory.
