import java.io.*;
import java.net.*;
import java.lang.*;
import java.util.*;
import java.sql.*;
import java.math.*;
import java.util.ArrayList;
import java.nio.charset.Charset;
import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.util.concurrent.CopyOnWriteArrayList;

class GetLocalOS
{
    static c_char   OS;
    static c_char   valid;
    GetLocalOS(){
        OS = new c_char(20); valid = new c_char(1);
    }

    public static int execute(String IP, int port)throws Exception{
        try{
            Socket socket = new Socket(IP, port);
            OutputStream socketOutputStream = socket.getOutputStream();
            try{
                socketOutputStream.write(setHeader());
            }catch(IOException ex){
                System.out.println("sent bytes failed");
                System.exit(-2);
            }
            byte b[] = new byte[21];
            InputStream socketInputStream = socket.getInputStream();
            int ret = socketInputStream.read(b);
            if(ret == -1 || ret == 0){
              System.out.println("no bytes read!");
              //java.lang.System.exit(1);
              return -1; // no bytes read return -1.
            }
            byte[] temp = new byte[20]; // temp for 4 byte integer
            for (int i = 0; i < 20 ; i++) {
              temp[i] = b[i];
            }
            valid.setValue(b[20]); // set t or f   (ture or false:only 1 byte)
            OS.setValue(temp); // set 4 bytes integer time(since 1970s)
        } catch (IOException e) {
            System.out.println("Could not listen on port 7777");
            System.exit(-1);
        }
        return 1; // some bytes read return 1.
    }

    private static byte[] setHeader() throws Exception{

      int length = OS.getSize()+valid.getSize();
      String cmd = "GetLocalOS";
      byte[] header = new byte[100+4+length];
      byte[] cmdbt = new byte[100];
      cmdbt = cmd.getBytes();
      for(int i = 0; i < 100; i++){  //set header 0-99 = "GetLocalTime"
        if(i < cmdbt.length) header[i] = cmdbt[i];
        else header[i] = 0;
      }

      //set 100-103: CmdLength
      byte[] length_bt = my_int_to_4bytes_array(length);
      for(int i = 100, j = 0; i <104 ; i++,j++){  // pass int
        header[i] = length_bt[j];
        //System.out.println("int is :"+header[i]);
      }

      //set 104 - dynamic length :CmdBuf
      //time.setValue(1);valid.setValue(2); test okay!
      byte[] temp1 = OS.toByte(), temp2 = valid.toByte();
      for(int i = 104, j = 0; i < 104+temp1.length; i++,j++){
        if((i-104) < temp1.length) header[i] = temp1[j];
        else header[i] = 0;
      }
      int offset = 104+temp1.length;
      for(int i = offset, j = 0; i < offset+temp2.length; i++,j++){
        if((i-offset) < temp1.length) header[i] = temp1[j];
        else header[i] = 0;
      }


      return header;
    }

    public static byte[] my_int_to_4bytes_array(int myInteger){
      return ByteBuffer.allocate(4).order(ByteOrder.LITTLE_ENDIAN).putInt(myInteger).array();
    }
}
