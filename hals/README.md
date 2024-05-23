```bash
export JNI_HOME=/lib/jvm/java-11-openjdk-amd64/
g++ -c -fPIC -I${JNI_HOME}/include -I${JNI_HOME}/include/linux LedController.cpp -o LedController.o
g++ -shared -fPIC -o libnative.so LedController.o -lc
```

```bash
javac -d . HelloWorldJNI.java
java -cp . -Djava.library.path=/home/bhien/jni com.baeldung.jni.HelloWorldJNI
```
