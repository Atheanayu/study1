package multipleThreadClient;

import java.io.*;
import java.net.Socket;
import java.util.concurrent.locks.Lock;
import java.util.concurrent.locks.ReentrantLock;

public class MultipleThreadClient extends Thread{
    private int currentPort;
    private Lock lock = new ReentrantLock();
    private String path;
    private int startPos, length;


    public MultipleThreadClient(int currentPort, int startPos, int length, String path){
        this.currentPort = currentPort;
        this.startPos = startPos;
        this.length = length;
        this.path = path;
    }

    /**
     * read msg from file
     * @return the byte array of the msg
     */
    public byte[] readFromFile(){
        try {
            byte[] buffer = new byte[length];
            RandomAccessFile file = new RandomAccessFile(this.path, "r");
            file.seek(startPos);
            file.read(buffer);
            file.close();
            return buffer;
        }catch (FileNotFoundException e){
            e.printStackTrace();
        }catch (IOException e){
            e.printStackTrace();
        }
        return null;
    }

    /**
     * read msg from file
     * and send it to server
     */
    @Override
    public void run() {
        try {
            lock.lock();
            Socket client = new Socket("localhost", this.currentPort);
            lock.unlock();
            DataOutputStream outputStream = new DataOutputStream(client.getOutputStream());
            DataInputStream inputStream = new DataInputStream(client.getInputStream());
            byte[] buffer = readFromFile();
            outputStream.write(buffer);

            outputStream.close();
            inputStream.close();
            client.close();
        }catch (IOException e){
            e.printStackTrace();
        }
    }


    public static void main(String[] args) {
        int portNum = 5; //open 5 ports
        int mainPort = 8000;
        int[] port = new int[]{8001, 8002, 8003, 8004, 8005};
        String filePath = "/Users/athena/IdeaProjects/Glutton_03/src/main/java/multipleThreadClient/clientFile01";
        File file = new File(filePath);
        int length = (int)file.length()/(portNum-1);
//        System.out.println(length);
        //here, it is used to communicate with server about the length of one port can get
        try {
            Socket client = new Socket("localhost", mainPort);
            DataOutputStream outputStream = new DataOutputStream(client.getOutputStream());
            DataInputStream inputStream = new DataInputStream(client.getInputStream());
            outputStream.writeUTF("length: "+length);
            System.out.println("msg from server: "+inputStream.readUTF());

            inputStream.close();
            outputStream.close();
            client.close();
        }catch (IOException e){
            e.printStackTrace();
        }

        for(int i = 0;i < portNum;i++) {
            MultipleThreadClient multipleThreadClient = new MultipleThreadClient(port[i], i*length, length,
                    filePath);
            multipleThreadClient.start();
        }
    }
}
