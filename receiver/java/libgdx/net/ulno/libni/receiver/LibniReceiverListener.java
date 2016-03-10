package net.ulno.libni.receiver;

/**
 * Created by ulno on 22.02.16.
 */
public interface LibniReceiverListener {
    public void buttonUpdated(int buttonNr, boolean pressed);
    public void analogUpdated(int analogNr, long value);
}