package multipleThreadServer;

import java.io.*;
import java.net.ServerSocket;
import java.net.Socket;

public class MultipleThreadServer extends Thread{
    private int currentPort; // which port
    private int startPos, length; // used to get the position in file
    private String path; // get the file path


    /**
     * constructor, it is used to initialize the private value
     */
    public MultipleThreadServer(int currentPort,int startPos, int length, String path){
        this.currentPort = currentPort;
        this.length = length;
        this.startPos = startPos;
        this.path = path;
    }

    /**
     * put the byte array to file, use RandomAccessFile to move file pointer
     * use the this.startPos and this.length to get the position of file pointer
     * @param buffer byte array
     */
    public void writeToFile(byte[] buffer){
        try {
            RandomAccessFile file = new RandomAccessFile(this.path, "rw");
            file.seek(startPos);// move the file pointer to startPos
            file.write(buffer);// write to file
        }catch (FileNotFoundException e){
            e.printStackTrace();
        }catch (IOException e){
            e.printStackTrace();
        }
    }

    /**
     * This program is a multiple thread program, this class extends Thread and override run function
     * in this function, it open a port and get the msg from client and then write to the file
     */
    @Override
    public void run() {
        byte[] buffer = new byte[this.length];
        try {
            ServerSocket serverSocket = new ServerSocket(this.currentPort);
            Socket server = serverSocket.accept();
            DataInputStream inputStream = new DataInputStream(server.getInputStream());
            DataOutputStream outputStream = new DataOutputStream(server.getOutputStream());
            inputStream.read(buffer);
            System.out.println("msg from client: "+ new String(buffer));
            writeToFile(buffer);

            inputStream.close();
            outputStream.close();
            server.close();
            serverSocket.close();
        }catch (IOException e){
            e.printStackTrace();
        }

    }

    public static void main(String[] args) {
        int portNum = 5; //open 5 ports
        int mainPort = 8000;
        int[] port = new int[]{8001, 8002, 8003, 8004, 8005};
        int length = 0;
        // here, this is used to communicate with client to get the length of one port can get
        try {
            ServerSocket serverSocket = new ServerSocket(mainPort);
            Socket server = serverSocket.accept();
            DataInputStream inputStream = new DataInputStream(server.getInputStream());
            DataOutputStream outputStream = new DataOutputStream(server.getOutputStream());
            length = Integer.parseInt(inputStream.readUTF().split(" ")[1]);// get the length
//            System.out.println(length);
            outputStream.writeUTF("get it");

            inputStream.close();
            outputStream.close();
            server.close();
            serverSocket.close();

        }catch (IOException e){
            e.printStackTrace();
        }
        for(int i = 0;i < portNum;i++) {
            MultipleThreadServer multipleThreadServer = new MultipleThreadServer(port[i],i*length, length,
                    "/Users/athena/IdeaProjects/Glutton_03/src/main/java/multipleThreadServer/serverFile01");
            multipleThreadServer.start();
        }
    }

}
