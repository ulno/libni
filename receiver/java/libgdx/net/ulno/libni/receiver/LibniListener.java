package net.ulno.libni.receiver;

/**
 * Created by ulno on 06.02.16.
 */
public interface LibniListener {
    /**
     * This is called, when a button was pressed and going down.
     * @param button
     */
    public void libniDown(int button );

    /**
     * This is called, when a button is released and going up.
     * @param button
     */
    public void libniUp(int button );

    /**
     * a 2 byte analog value is sent for controller with nr analogNr
     * values from -2^15 to 2^15-1
     * @param analogNr
     * @param value
     */
    public void libniAnalog(int analogNr, long value);
}
