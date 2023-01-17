set OSNAME=eta-os
set BUILDDIR=%0/../bin
set OVMFDIR=%0/../../ovmf

qemu-system-x86_64 -machine q35 -drive format=raw,file=%BUILDDIR%/%OSNAME%.img -m 256M -cpu qemu64 -drive if=pflash,format=raw,unit=0,file="%OVMFDIR%/OVMF_CODE.fd",readonly=on -drive if=pflash,format=raw,unit=1,file="%OVMFDIR%/OVMF_VARS.fd" -net none
pause
