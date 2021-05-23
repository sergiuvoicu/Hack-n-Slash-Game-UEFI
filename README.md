# Hack'n'Slash UEFI
## Description
A Hack'n'Slash UEFI bootloader game developed in C, using the GNU-EFI library and OVMF. The project is developed in Linux.

## Examples
Nuclear Throne: https://www.youtube.com/watch?v=n3IvM0f2Lvw
The Binding of Isaac: https://www.youtube.com/watch?v=Sr4LReuBfhw
Restless Dungeon: https://www.youtube.com/watch?v=RQ9dRZHqwmM

## Installation
### Set-up
**sudo apt-get install gnu-efi**
<br>
**sudo apt-get install ovmf**
<br>
**sudo apt-get install qemu-system-x86**
<br>

mkdir hacknslash
<br>
cd hacknslash
<br>
cp /usr/share/ovmf/OVMF.fd bios.bin
<br>
git clone https://gitlab.com/smp2021/332aa/hack-n-slash-uefi-bootloader.git
### Compile
cd hack-n-slash-uefi-bootloader
make
### Run 
cd hacknslash
qemu-system-x86_64 -pflash bios.bin -hda fat:rw:hack-n-slash-uefi-bootloader -net none

Functionalitati: mai multe tipuri de inamici melee/ranged, mod de lupta player ranged, trei runde, trei vieti, blocuri untargetable
