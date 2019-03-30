# qt_components
Self Developed, Indenpent Components

## Componets List

### Communicator

A simple, useful communicator aims at simplify communicating between PCs and MCUs, it contains SerialPort, Network, Bluetooth (although not finished yet) and a simple Virtual COM (it is uncomplete like Bluthooth, but can simulate a basic com).

[Here](https://brifuture.github.io/qt_components/basic_communicator/docs) is a simple documents for it.

**Newly Added**

* Improve AbstractProtocol with CommandObject, AbstractProtocol can define command execute times and timeout, CommManager can control the process of command repeat and timeout. When interacting with UI, the UI layer just use a subclass instance of CommandObject will be fine.

**TODO**

* ~~Use a xml that can be used by VirtualCom, Let VirtualCom choose a certain XML to _simulate a Certain MCU_, which means that it can completely response as a real MCU do when receiving a command by caller.~~ The functionaty of VirtualCom can be made by a python scripts, see [BriFuture/sim.py](https://gist.github.com/BriFuture/4b3f747cfb91d480d0f1e14629f86450) for more information.
* Finish Bluetooth feature. In fact, My Computer does not support Bluetooth, it is a little hard for me to add this feature.

#### update

Make a new lib project named `BasicComm` which preformed as the same as basic_communicator but it provide more convenient ways to build other software which would dependent `BasicComm`, read more from [BasicComm](./BasicComm/README.md)

---------

## About The code

Code in this repository is under [GPLv3 License](./license).