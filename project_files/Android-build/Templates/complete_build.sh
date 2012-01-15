#! /bin/sh


cd SDL-android-project
${ANDROID_NDK}/ndk-build -j 8
if [ $? -ne 0 ] 
then 
  echo "Failed to execute ${ANDROID_NDK}/ndk-build"
  exit 1
fi

cd ..
make -f Makefile.android
if [ $? -ne 0 ] 
then 
  echo "Failed to execute make"
  exit 1
fi

cd SDL-android-project
ant install
if [ $? -ne 0 ] 
then 
  echo "Failed to execute ant install"
  exit 1
fi
exit 0 
