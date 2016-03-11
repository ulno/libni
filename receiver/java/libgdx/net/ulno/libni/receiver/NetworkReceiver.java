package net.ulno.libni.receiver;

/**
 * Created by ulno on 06.02.16.
 *
 * receive control-commands via Network (udp, tcp, or mqtt)
 * eventually also send status updates back
 */
public class NetworkReceiver extends LibniReceiver {
    private boolean buttonStates[];
    private int analogStates[];
    NetworkReceiverID id;

    public NetworkReceiver(NetworkReceiverID networkReceiverID) {
        super();
        this.id = this.id;
        buttonStates = new boolean[NUMBER_OF_BUTTONS];
        for (int i = 0; i < NUMBER_OF_BUTTONS; i++) buttonStates[i] = false;
        analogStates = new int[NUMBER_OF_ANALOGS];
        for (int i = 0; i < NUMBER_OF_ANALOGS; i++) analogStates[i] = 0;
    }

    /**
     * Get the network package which belongs to this NetworkReceiver
     * This will actually call the listener, if something has changed
     */
    public void evaluateNetworkPackage(byte[] messageSaved, int messageSavedSize ) {
        // see if packages arrived
        if (libniControllerListener != null) { // if initialized and somebody is subscribed
            int pointer = 0;
            pointer += NetworkMultiplexer.BUFFER_HEADER_SIZE;
            // parse header
            // look at protocol type
            switch (((messageSaved[8] & 255) << 8) + messageSaved[9]) {
                case 0: // bitblock
                    int buttonNr = 0;
                    while(buttonNr < NUMBER_OF_BUTTONS) {
                        byte current = messageSaved[pointer];
                        for(int bit=0; bit<8; bit++) {
                            buttonEvent (buttonNr, (current & 1) == 1 );
                            current >>= 1; // shift right
                            buttonNr ++;
                        }
                        pointer ++;
                    }
                    int analogNr = 0;
                    while(analogNr < NUMBER_OF_ANALOGS) {
                        int newState = (messageSaved[pointer] << 8) + messageSaved[pointer+1];
                        if (newState != analogStates[analogNr]) {
                            analogStates[analogNr] = newState;
                            registerAnalogEvent(analogNr);
                        }
                        pointer += 2;
                        analogNr++;
                    }
                    break;
                case 1: // events
                    // parse last message received and compare with actual states
                    byte button;
                    while (pointer <= messageSavedSize - 2) {
                        int event = messageSaved[pointer];
                        pointer++;
                        switch (event) {
                            case 1: // Button Up
                                buttonEvent(messageSaved[pointer], false);
                                pointer++;
                                break;
                            case 2: // Button Down
                                buttonEvent(messageSaved[pointer], true);
                                pointer++;
                                break;
                            case 3:
                                button = messageSaved[pointer];
                                pointer++;
                                byte high = messageSaved[pointer];
                                pointer++;
                                int low = ((int)messageSaved[pointer]) & 0xff;
                                int newState = ((high >= 128) ? (256 - high) : high) * 256 + low; // little endian two -complement
                                if (newState != analogStates[button]) {
                                    analogStates[button] = newState;
                                    registerAnalogEvent(button);
                                }
                                pointer++;
                        } // switch event type
                    }
            } // switch protocol type
        }
    }

    private void buttonEvent(int buttonNr, boolean newState) {
        boolean oldState = buttonStates[buttonNr];
        if (oldState != newState) {
              buttonStates[buttonNr] = newState;
            registerButtonEvent(buttonNr);
        }
    }


    @Override
    protected boolean unmappedButtonPressed(int unmappedButtonNr) {
        return buttonStates[unmappedButtonNr];
    }

    @Override
    protected long unmappedAnalog(int unmappedAnalogNr) {
        return analogStates[unmappedAnalogNr];
    }

}
