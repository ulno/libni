package net.ulno.nii;

import java.awt.*;
import java.awt.event.KeyEvent;
import java.io.*;

/**
 * Created by ulno on 24.02.16.
 */
public class Main {

    public static void main(String [] args) {
        System.out.println("Network input injector started.");
        LibniHandler lh = new LibniHandler();
        lh.run();
    }
}
