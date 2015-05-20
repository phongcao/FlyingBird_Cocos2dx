# FlyingBird_Cocos2dx
A "Flappy Bird" clone developed using Cocos2d-x. Targeting Windows 10 UAP.

How to build
------------

1. Clone the repo from GitHub:

         $ git clone https://github.com/phongcao/FlyingBird_Cocos2dx.git
         
2. Clone cocos2d-x supporting Windows 10 UAP from MSOpenTech's repo:

         $ cd FlyingBird_Cocos2dx
         $ git clone https://github.com/MSOpenTech/cocos2d-x.git cocos2d         
         $ cd cocos2d
         $ git checkout v3.6-uap  

3. After cloning the repo, please execute `download-deps.py` to download and install dependencies:

         $ python download-deps.py

4. After running `download-deps.py`:

         $ git submodule update --init
         
5. Open `proj.win10/FlyingBird_Cocos2dx.sln` to build and run.
