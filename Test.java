import java.lang.System;
import java.lang.String;
import java.nio.ByteBuffer;
import java.nio.ByteOrder;
public class Test{
  public static void main(String args[])throws Exception{
//testing GetLocalTime
    String IP = "127.0.0.1";
    int port = 7777;
    GetLocalTime obj = new GetLocalTime();
    obj.valid.setValue(false);
    obj.execute(IP, port);

    byte[] t = obj.time.getValue(), a = obj.valid.getValue();
    ByteBuffer bb = ByteBuffer.wrap(t);
    bb.order(ByteOrder.BIG_ENDIAN);

    System.out.println("time: "+bb.getInt()+"s since 1970s Jan 1th");
    if((new String(a)).equals("T")) System.out.println("Valid : True"); // c side pass valid value: T
    else System.out.println("Valid : false");

    //System.out.printf("time : "+t+"| valid: "+a);


    GetLocalOS obj1 = new GetLocalOS();
    obj.valid.setValue(false);
    obj1.execute(IP, port);
    byte[] m = obj1.OS.getValue(), n = obj1.valid.getValue();
    System.out.println("OS : "+new String(m));
    if((new String(n)).equals("T")) System.out.println("Valid : True");

  }
}
