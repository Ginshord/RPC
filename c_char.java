import java.lang.*;
import java.util.*;
class c_char
{
    byte[] buf = new byte[1]; // little endian
    boolean has_value = false;
    c_char(int index_size){
      if (index_size!=1) this.buf = new byte[index_size];
    }

    public int getSize(){// get byte size : how many byte has been set values.
      return buf.length;   /* output 4 : 4bytes  =  int. thorugh int from c can convert to ascii char in java.
                          in c char[] bt; 1byte = 0-255; bt[1] = 'z'-'a'+ 97 bt[1]= (int)z   pass to java
                          then java convert such int in a byte to String
                          through String str = new String(header, "US-ASCII");   */
    }
    public byte[] getValue(){
      if(has_value == false) {
          String str ="0000";
          buf = str.getBytes();
          return buf;
      }
      else return buf;
    }

    public void setValue(byte[] buf){
      has_value =true;
      this.buf=buf;
    }
    public void setValue(boolean f){
      has_value = f;
    }
    public void setValue(byte v){  //int v convert to 4byte. this assignment dont have parameters. no need to implement
      has_value =true;
      this.buf[0] = v;
    }
    public byte[] toByte(){
      return buf;
    }
}
