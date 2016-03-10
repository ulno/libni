package net.ulno.libni.gdxReceiver;

import com.badlogic.gdx.Input;
import net.ulno.libni.receiver.LibniMapping;

/**
 * Created by ulno on 22.02.16.
 */
public class GdxKeyboardMapping extends LibniMapping {
    @Override
    public GdxKeyboardMapping addButton(int libniButton, int keycode) {
        return (GdxKeyboardMapping) super.addButton(libniButton, keycode);
    }

    @Override
    public GdxKeyboardMapping addButtonFromAnalog(int libniButton, int analogNr, boolean positive) {
        return (GdxKeyboardMapping) super.addButtonFromAnalog(libniButton, analogNr, positive);
    }

    @Override
    public GdxKeyboardMapping addAnalog(int libniAnalog, int analogNr) {
        return (GdxKeyboardMapping) super.addAnalog(libniAnalog, analogNr);
    }

    /**
     * Just map normal chars and cursor keys to the respective libni-keys
     */
    @Override
    public void initDefault() {
        addButton('a', Input.Keys.A);
        addButton('b', Input.Keys.B);
        addButton('c', Input.Keys.C);
        addButton('d', Input.Keys.D);
        addButton('e', Input.Keys.E);
        addButton('f', Input.Keys.F);
        addButton('g', Input.Keys.G);
        addButton('h', Input.Keys.H);
        addButton('i', Input.Keys.I);
        addButton('j', Input.Keys.J);
        addButton('k', Input.Keys.K);
        addButton('l', Input.Keys.L);
        addButton('m', Input.Keys.M);
        addButton('n', Input.Keys.N);
        addButton('o', Input.Keys.O);
        addButton('p', Input.Keys.P);
        addButton('q', Input.Keys.Q);
        addButton('r', Input.Keys.R);
        addButton('s', Input.Keys.S);
        addButton('t', Input.Keys.T);
        addButton('u', Input.Keys.U);
        addButton('v', Input.Keys.V);
        addButton('w', Input.Keys.W);
        addButton('x', Input.Keys.X);
        addButton('y', Input.Keys.Y);
        addButton('z', Input.Keys.Z);
        addButton('0', Input.Keys.NUM_0);
        addButton('1', Input.Keys.NUM_1);
        addButton('2', Input.Keys.NUM_2);
        addButton('3', Input.Keys.NUM_3);
        addButton('4', Input.Keys.NUM_4);
        addButton('5', Input.Keys.NUM_5);
        addButton('6', Input.Keys.NUM_6);
        addButton('7', Input.Keys.NUM_7);
        addButton('8', Input.Keys.NUM_8);
        addButton('9', Input.Keys.NUM_9);
        addButton(16, Input.Keys.UP); // ctrl-p (Previous)
        addButton(14, Input.Keys.DOWN); // ctrl-n (Next)
        addButton(2, Input.Keys.LEFT); // ctrl-b (Back)
        addButton(6, Input.Keys.RIGHT); // ctrl-f (Forward)
    }

}
