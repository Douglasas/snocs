# Steps to compile for Ubuntu

```
sudo apt install build-essential
sudo apt install libsystemc libsystemc-dev
```

Modify the `common.pri` file and indicate the systemc root directory. In the ubuntu package, it will be at `/usr/local/systemc`.

```
qmake
make
```

If everything worked, the file `Simulator/SNoCS` used by the RedScarf simulator.

Create a link for the parameters library in the `Simulator/` folder

```
cd Simulator/
ln -s ../plugins/libparameters.so.1
```
