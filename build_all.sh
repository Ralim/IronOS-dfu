!# bin/sh

make clean
make -j build_type=bootloader model=TS100
rm -rf build/*.o build/*.d
make -j build_type=runtime model=TS100
rm -rf build/*.o build/*.d

make -j build_type=bootloader model=TS80
rm -rf build/*.o build/*.d
make -j build_type=runtime model=TS80
rm -rf build/*.o build/*.d

make -j build_type=bootloader model=TS80P
rm -rf build/*.o build/*.d
make -j build_type=runtime model=TS80P
rm -rf build/*.o build/*.d

make -j build_type=bootloader model=MHP30
rm -rf build/*.o build/*.d
make -j build_type=runtime model=MHP30
rm -rf build/*.o build/*.d

make -j build_type=bootloader model=S60P
rm -rf build/*.o build/*.d
make -j build_type=runtime model=S60P
rm -rf build/*.o build/*.d
