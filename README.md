# Hack'n'Slash UEFI
## Description
A Hack'n'Slash UEFI bootloader game developed in C, using the GNU-EFI library and OVMF. The project is developed in Linux. <br>
The goal of the player is to reach and defeat all the enemies on contact, while avoiding to get hit by their bullets. 

## Examples
Nuclear Throne: https://www.youtube.com/watch?v=n3IvM0f2Lvw <br>
The Binding of Isaac: https://www.youtube.com/watch?v=Sr4LReuBfhw <br>
Restless Dungeon: https://www.youtube.com/watch?v=RQ9dRZHqwmM <br>

## Installation
### Set-up
**sudo apt-get install gnu-efi** <br>
**sudo apt-get install ovmf** <br>
**sudo apt-get install qemu-system-x86** <br>

mkdir hacknslash <br>
cd hacknslash <br>
cp /usr/share/ovmf/OVMF.fd bios.bin <br>
git clone https://gitlab.com/smp2021/332aa/hack-n-slash-uefi-bootloader.git <br>
### Compile
cd hack-n-slash-uefi-bootloader <br>
make
### Run 
cd hacknslash <br>
qemu-system-x86_64 -pflash bios.bin -hda fat:rw:hack-n-slash-uefi-bootloader -net none <br> <br>
![](psmp1.png)*How to run the game under QEMU* <br>
## Functionalities
- Player controlled on key input: **w/a/s/d** <br> 
- Untargetable obstacles, randomly generated, conditioned by player's position <br>
- Enemies, randomly generated, conditioned by player's and obstacles' position <br>
- Melee player attack system which deletes the enemies in contact with the player on key input **m** <br>
- Timed enemies position change <br>
- Ranged enemies attack system which damages the player's life status <br>
- Timed fire rate <br>
- Obstacles collision system: blocking the player and the enemies and deleting the bullets <br>
- Three levels with ascending difficulty <br>
- Life status updated on-hit <br>
- Interactive GUI using Graphics Output Protocol and Simple Text Output Protocol <br> <br>
![](psmp2.png)*First round* <br>
![](psmp3.png)*In-between rounds message* <br>
![](psmp4.png)*Last round* <br>
![](psmp5.png)*Defeat message* <br>
![](psmp6.png)*Victory message* <br>
