package net.ulno.libni.gdxReceiver;

import com.badlogic.gdx.Gdx;
import net.ulno.libni.receiver.LibniReceiver;
import net.ulno.libni.receiver.LibniMapping;

/**
 * Created by ulno on 22.02.16.
 */
public class GdxKeyboardReceiver extends LibniReceiver {
    GdxKeyboardReceiver() {
        super(new GdxKeyboardMapping());
    }

    public GdxKeyboardReceiver(LibniMapping keyboardMapping) {
        super(keyboardMapping);
    }

    @Override
    protected boolean unmappedButtonPressed(int keycode) {
        return Gdx.input.isKeyPressed(keycode);
    }

    @Override
    protected long unmappedAnalog(int unmappedAnalogNr) {
        return 0;
    }

}
