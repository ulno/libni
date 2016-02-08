package net.ulno.gamenetcontroller;

import com.badlogic.gdx.ApplicationAdapter;
import com.badlogic.gdx.Gdx;
import com.badlogic.gdx.Input;
import com.badlogic.gdx.InputProcessor;
import com.badlogic.gdx.graphics.GL20;
import com.badlogic.gdx.graphics.Texture;
import com.badlogic.gdx.graphics.g2d.SpriteBatch;

import java.io.IOException;
import java.net.DatagramPacket;
import java.net.DatagramSocket;
import java.net.InetAddress;

public class Main extends ApplicationAdapter {
    //public static final String DESTINATION_HOST = "ulno-work";
//    public static final String DESTINATION_HOST = "localhost";
    public static final String DESTINATION_HOST = "192.168.15.194";
    public static final int DESTINATION_PORT = 19877;

    public static final int NUMBER_OF_BUTTONS = 256;
    public static final int NUMBER_OF_BUTTON_BYTES = (NUMBER_OF_BUTTONS + 7)/8;
    public static final int NUMBER_OF_AXIS = 16;
    public static final int NUMBER_OF_AXIS_BYTES = NUMBER_OF_AXIS * 2;
    private byte message[] = new byte[NUMBER_OF_BUTTON_BYTES + NUMBER_OF_AXIS_BYTES];

    SpriteBatch batch;
    Texture img;

    private DatagramSocket socket=null;
    DatagramPacket datagramPacket;
    private long frames=0;

    private void setButton(int nr, boolean isPressed) {
        int byteNr = nr / 8;
        int bitNr = nr % 8;
        if( isPressed ) {
            message[byteNr] |= 1 << bitNr;
        } else {
            message[byteNr] &= 255 - (1<<bitNr);
        }
    }

    private boolean getButton( int nr ) {
        int byteNr = nr / 8;
        int bitNr = nr % 8;
        return (message[byteNr] & (1<<bitNr))>0;
    }

    private void setAxis(int nr, int value) {
        int byteNr = NUMBER_OF_BUTTONS + nr * 2;
        byte high = (byte)((value & 0xffff) >> 8);
        byte low = (byte)(value & 0xff);
        message[byteNr] = high;
        message[byteNr+1] = low;
    }

    private int getAxis(int nr) {
        int byteNr = NUMBER_OF_BUTTON_BYTES + nr * 2;
        byte high = message[byteNr];
        byte low = message[byteNr+1];
        return  ((high >= 128) ? (256 - high) : high) * 256 + low; // little endian two -complement
    }

    private void send() {
        try {
            socket.send(datagramPacket);
        } catch (IOException e) {
            e.printStackTrace();
        }
    }

    @Override
    public void create() {
        batch = new SpriteBatch();
        img = new Texture("badlogic.jpg");

        for(int i=0; i<message.length; i++) message[i] = 0; // init

        try {
            socket = new DatagramSocket();
            InetAddress host = InetAddress.getByName(DESTINATION_HOST);
            datagramPacket = new DatagramPacket(message,message.length,host,DESTINATION_PORT);
        } catch (Exception e) {
            e.printStackTrace();
        }

        Gdx.input.setInputProcessor(new InputProcessor() {
            @Override
            public boolean keyDown(int keycode) {
                sendButton(true, keycode);
                return false;
            }

            @Override
            public boolean keyUp(int keycode) {
                sendButton(false, keycode);
                return false;
            }

            @Override
            public boolean keyTyped(char character) {
                return false;
            }

            @Override
            public boolean touchDown(int screenX, int screenY, int pointer, int button) {
                return false;
            }

            @Override
            public boolean touchUp(int screenX, int screenY, int pointer, int button) {
                return false;
            }

            @Override
            public boolean touchDragged(int screenX, int screenY, int pointer) {
                return false;
            }

            @Override
            public boolean mouseMoved(int screenX, int screenY) {
                return false;
            }

            @Override
            public boolean scrolled(int amount) {
                return false;
            }
        });
    }

    private void sendButton(boolean isPressed, int keycode) {
        char send=0;
        switch (keycode) {
            case Input.Keys.UP:
            case Input.Keys.W:
                send = 'w';
                break;
            case Input.Keys.RIGHT:
            case Input.Keys.D:
                send = 'd';
                break;
            case Input.Keys.DOWN:
            case Input.Keys.S:
                send = 's';
                break;
            case Input.Keys.LEFT:
            case Input.Keys.A:
                send = 'a';
                break;
            case Input.Keys.SPACE:
                send = ' ';
                break;
            case Input.Keys.NUM_0:
                send = '0';
                break;
            case Input.Keys.NUM_1:
                send = '1';
                break;
            case Input.Keys.NUM_2:
                send = '2';
                break;
            case Input.Keys.NUM_3:
                send = '3';
                break;
            case Input.Keys.NUM_4:
                send = '4';
                break;
            case Input.Keys.NUM_5:
                send = '5';
                break;
            case Input.Keys.NUM_6:
                send = '6';
                break;
            case Input.Keys.NUM_7:
                send = '7';
                break;
            case Input.Keys.NUM_8:
                send = '8';
                break;
            case Input.Keys.NUM_9:
                send = '9';
                break;
            case Input.Keys.ESCAPE:
                send = 27;
                break;
        }
        if(send > 0) {
            setButton(send,isPressed);
            send();
        }
    }

    @Override
    public void render() {
        frames ++;
        if(frames > 5) {
            frames = 0;
            send(); // send updates all the time
        }
        Gdx.gl.glClearColor(1, 0, 0, 1);
        Gdx.gl.glClear(GL20.GL_COLOR_BUFFER_BIT);
        batch.begin();
        batch.draw(img, 0, 0);
        batch.end();
    }
}
