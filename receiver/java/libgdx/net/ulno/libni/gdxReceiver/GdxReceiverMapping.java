package net.ulno.libni.gdxReceiver;

import net.ulno.libni.receiver.LibniMapping;

/**
 * Created by ulno on 23.02.16.
 */
public class GdxReceiverMapping extends LibniMapping {
    @Override
    public GdxReceiverMapping addButton(int libniButton, int keycode) {
        return (GdxReceiverMapping) super.addButton(libniButton, keycode);
    }

    @Override
    public GdxReceiverMapping addButtonFromAnalog(int libniButton, int analogNr, boolean positive) {
        return (GdxReceiverMapping) super.addButtonFromAnalog(libniButton, analogNr, positive);
    }

    @Override
    public GdxReceiverMapping addAnalog(int libniAnalog, int analogNr) {
        return (GdxReceiverMapping) super.addAnalog(libniAnalog, analogNr);
    }
}

