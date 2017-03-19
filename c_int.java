import java.lang.*;
import java.util.*;
class c_int
{
    byte[] buf = new byte[4]; // little endian
    boolean has_value = false;

    public int getSize(){ // get byte size : how many byte has been set values.
        return buf.length;
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
      has_value = true;
      this.buf = buf;
    }
    // public void setValue(int v){
    //   has_value =true;
    //   String str ="1111";
    //   buf = str.getBytes();
    //
    // }
    public byte[] toByte(){
      return buf;
    }
}
