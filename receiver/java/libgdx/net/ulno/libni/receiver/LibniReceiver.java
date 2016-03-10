package net.ulno.libni.receiver;

import java.util.ArrayList;

/**
 * Class needs to be extended for the rigth controller type (Keyboard, GdxController, Network - udp, tcp, or MQTT)
 * Created by ulno on 22.02.16.
 */
public abstract class LibniReceiver {
    public static final int NUMBER_OF_BUTTONS = 256;
    public static final int NUMBER_OF_ANALOGS = 16;

    LibniMapping mapping;
    LibniReceiverListener libniControllerListener = null;

    private boolean[] buttons = new boolean[NUMBER_OF_BUTTONS];
    private long[] analogs = new long[NUMBER_OF_ANALOGS];

    public void init(LibniMapping mapping) {
        for(int i = 0; i > NUMBER_OF_BUTTONS; i++) {
            buttons[i] = false;
        }
        for(int i = 0; i > NUMBER_OF_ANALOGS; i++) {
            analogs[i] = 0;
        }
        this.mapping = mapping;
    }

    public LibniReceiver() {
        LibniMapping mapping = new LibniMapping();
        mapping.initDefault();
        init(mapping);
    }

    public LibniReceiver(LibniMapping mapping) {
        init(mapping);
    }

    public void setLibniControllerListener(LibniReceiverListener libniControllerListener) {
        this.libniControllerListener = libniControllerListener;
    }

    void clearUpdateListener() {
        libniControllerListener = null;
    }

    /**
     * This should usually be overwritten by the extending class
     *
     * @param unmappedButtonNr
     * @return
     */
    protected abstract boolean unmappedButtonPressed( int unmappedButtonNr );

    protected abstract long unmappedAnalog( int unmappedAnalogNr );

    private boolean unmappedAnalogToLibniButton(int unmappedAnalogNr, int libniButtonNr) {
        long analogValue = unmappedAnalog(unmappedAnalogNr);
        return mapping.selectButtonValue(unmappedAnalogNr, libniButtonNr, analogValue);
    }

    /**
     * This is called when there was a button change happening through a registered event
     * @param buttonNr
     * @param pressed button changed to pressed-state
     */
    private void buttonUpdated( int buttonNr, boolean pressed ) {
        if(libniControllerListener != null) {
            libniControllerListener.buttonUpdated( buttonNr, pressed );
        }
    }

    /**
     * This is called when there was a ananlog change happening through a registered event
     * @param analogNr
     * @param value
     */
    private void analogUpdated( int analogNr, long value ) {
        if(libniControllerListener != null) {
            libniControllerListener.analogUpdated( analogNr, value );
        }
    }

    /**
     * needs to be called when an event was triggered for this
     * @param unmappedButtonNr
     */
    public void registerButtonEvent( int unmappedButtonNr ) {
        ArrayList<Integer> triggerButtons = mapping.getLibniButtonsFromUnmappedButton(unmappedButtonNr);
        if(triggerButtons == null) return; // is not handled
        int bsize = triggerButtons.size();
        if(bsize == 0) return;
        for(int triggerIndex = bsize-1; triggerIndex >= 0; triggerIndex--) {
            int button = triggerButtons.get(triggerIndex);
            processButtonEventForLibniButton( button );
        }
    }

    private void processButtonEventForLibniButton(int libniButton) {
        ArrayList<Integer> triggers = mapping.getUnmappedButtonsFromLibniButton(libniButton);
        boolean currentState = buttons[libniButton];
        boolean newState = false;
        if (triggers != null) for (int i = triggers.size() - 1; i >= 0; i--) {
            if (unmappedButtonPressed(triggers.get(i))) { // if any is pressed, say button is pressed
                newState = true;
                break;
            }
        }
        if (!newState) {
            // check also the analogs to compute state
            ArrayList<Integer> analogs = mapping.getUnmappedAnalogsFromLibniButton(libniButton);
            if (analogs != null) for (int i = analogs.size() - 1; i >= 0; i--) {
                if( unmappedAnalogToLibniButton(analogs.get(i), libniButton) ) {
                    newState = true;
                    break;
                }
            }
        }

        if (newState != currentState) {
            buttons[libniButton] = newState;
            buttonUpdated(libniButton, newState);
        }
    }

    /**
     * needs to be called when an event was triggered for this
    * @param unmappedAnalogNr
     */
    public void registerAnalogEvent( int unmappedAnalogNr ) {
        // check first if this analog influences buttons
        ArrayList<Integer> libniButtons = mapping.getLibniButtonsFromUnmappedAnalog(unmappedAnalogNr);
        if(libniButtons != null && libniButtons.size() > 0) { // we have some buttons which are influenced
            for(int i = libniButtons.size()-1; i>=0; i--) {
                int libniButton = libniButtons.get(i);
                processButtonEventForLibniButton( libniButton ); // just forward
            }
        }

        // now check analogs which are influenced
        int libniAnalog = mapping.getLibniAnalogFromUnmappedAnalog(unmappedAnalogNr);
        if(libniAnalog<0) return; // not handled

        long currentState = analogs[libniAnalog];
        long newState =  unmappedAnalog(unmappedAnalogNr);
        if(currentState != newState) {
            analogs[libniAnalog] = newState;
            analogUpdated(libniAnalog, newState);
        }
    }

    public void evaluate() {
        // usually empty but called from render to do some cooperative multitasking tasks here
    }

    public void dispose() {
        clearUpdateListener();
    }

    public boolean getButton(int button) {
        return buttons[button];
    }

    public long getAnalog(int analog) {
        return analogs[analog];
    }
}
