package net.ulno.nii;

import net.ulno.libni.receiver.LibniReceiverListener;
import net.ulno.libni.receiver.LibniMapping;
import net.ulno.libni.receiver.NetworkMultiplexer;
import org.yaml.snakeyaml.Yaml;

import java.awt.*;
import java.awt.event.KeyEvent;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.InputStreamReader;

/**
 * Created by ulno on 24.02.16.
 */
public class LibniHandler implements LibniReceiverListener {
    Robot robot = null;
    InputStreamReader reader = null;
    NetworkMultiplexer networkMultiplexer = null;
    LibniMapping mapping = null;

    public LibniHandler() {
        try {
            reader = new InputStreamReader(new FileInputStream("testconfig"));
        } catch (FileNotFoundException e) {
            e.printStackTrace();
        }

        networkMultiplexer = new NetworkMultiplexer(reader);
        networkMultiplexer.setLibniControllerListener(this);
        mapping = new LibniMapping()
                .addButton(LibniMapping.BUTTON_UP, KeyEvent.VK_UP)
                .addButton(LibniMapping.BUTTON_DOWN, KeyEvent.VK_DOWN)
                .addButton(LibniMapping.BUTTON_LEFT, KeyEvent.VK_LEFT)
                .addButton(LibniMapping.BUTTON_RIGHT, KeyEvent.VK_RIGHT)
                .addButton(LibniMapping.BUTTON_FIRE, KeyEvent.VK_SPACE)
                .addButton(LibniMapping.BUTTON_ESCAPE, KeyEvent.VK_ESCAPE)
                .addButton('0', KeyEvent.VK_0)
                .addButton('1', KeyEvent.VK_1)
                .addButton('2', KeyEvent.VK_2)
                .addButton('3', KeyEvent.VK_3)
                .addButton('4', KeyEvent.VK_4)
                .addButton('5', KeyEvent.VK_5)
                .addButton('6', KeyEvent.VK_6)
                .addButton('7', KeyEvent.VK_7)
                .addButton('8', KeyEvent.VK_8)
                .addButton('0', KeyEvent.VK_9)
                .addButton('a', KeyEvent.VK_A)
                .addButton('b', KeyEvent.VK_B)
                .addButton('c', KeyEvent.VK_C)
                .addButton('d', KeyEvent.VK_D)
                .addButton('e', KeyEvent.VK_E)
                .addButton('f', KeyEvent.VK_F)
                .addButton('g', KeyEvent.VK_G)
                .addButton('h', KeyEvent.VK_H)
                .addButton('i', KeyEvent.VK_I)
                .addButton('j', KeyEvent.VK_J)
                .addButton('k', KeyEvent.VK_K)
                .addButton('l', KeyEvent.VK_L)
                .addButton('m', KeyEvent.VK_M)
                .addButton('n', KeyEvent.VK_N)
                .addButton('o', KeyEvent.VK_O)
                .addButton('p', KeyEvent.VK_P)
                .addButton('q', KeyEvent.VK_Q)
                .addButton('r', KeyEvent.VK_R)
                .addButton('s', KeyEvent.VK_S)
                .addButton('t', KeyEvent.VK_T)
                .addButton('u', KeyEvent.VK_U)
                .addButton('v', KeyEvent.VK_V)
                .addButton('w', KeyEvent.VK_W)
                .addButton('x', KeyEvent.VK_X)
                .addButton('y', KeyEvent.VK_Y)
                .addButton('z', KeyEvent.VK_Z);

        System.out.println("Network Input Injector starting.");
        try {
            robot = new Robot();
        } catch (AWTException e) {
            e.printStackTrace();
        }
    }

    public void run() {
        while(true) {
            networkMultiplexer.evaluate();
            robot.delay(1); // prevent busy waiting
        }
    }

    @Override
    public void buttonUpdated(int buttonNr, boolean pressed) {
        int keyEvent = mapping.getUnmappedButtonsFromLibniButton(buttonNr).get(0);
        if(pressed) {
            robot.keyPress(keyEvent);
        } else {
            robot.keyRelease(keyEvent);
        }
    }

    @Override
    public void analogUpdated(int analogNr, long value) {

    }
}
