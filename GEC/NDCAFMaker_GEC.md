## Run most recent ND CAFMaker

https://github.com/DUNE/ND_CAFMaker#nd-cafmaker

```
mkdir NDCAF
git clone https://github.com/DUNE/ND_CAFMaker.git
./build.sh                                         # GEC code is built and path exported here
source ndcaf_setup.sh
make

# to run
./bin/makeCAF --fcl=./cfg/ndcafmakerjob.fcl
```
