package net.ulno.libni.gdxReceiver;

import com.badlogic.gdx.Gdx;
import com.badlogic.gdx.InputMultiplexer;
import com.badlogic.gdx.InputProcessor;
import com.badlogic.gdx.controllers.Controller;
import com.badlogic.gdx.controllers.ControllerListener;
import com.badlogic.gdx.controllers.Controllers;
import com.badlogic.gdx.controllers.PovDirection;
import com.badlogic.gdx.files.FileHandle;
import com.badlogic.gdx.math.Vector3;
import net.ulno.libni.receiver.*;

import java.util.ArrayList;

/**
 * Created by ulno on 22.02.16.
 *
 * Merge different kind of Inputs for to the Network Input Format
 */
public class GdxMergedInput implements InputProcessor, ControllerListener, LibniReceiverListener {

    ArrayList<LibniReceiver> libniReceivers = new ArrayList<>();
    private NewNetworkReceiverCallback newControllerCallback; // is called, when a new controller is detected
    private boolean[] mergedButtons = new boolean[LibniReceiver.NUMBER_OF_BUTTONS];
    private long[] mergedAnalogs = new long[LibniReceiver.NUMBER_OF_ANALOGS];
    private GdxKeyboardReceiver keyboardController = null;
    InputMultiplexer inputMultiplexer;
    NetworkMultiplexer networkMultiplexer;
    GdxReceiverMultiplexer gdxControllerMultiplexer;
    private LibniListener listener;

    void init(GdxKeyboardMapping gdxKeyboardMapping,
              NewNetworkReceiverCallback newNetworkControllerCallback, FileHandle networkConfig,
              GdxReceiverMapping gdxReceiverMapping) {
        this.newControllerCallback = newNetworkControllerCallback;
        initInputMultiplexer();
        keyboardController = new GdxKeyboardReceiver(gdxKeyboardMapping);
        keyboardController.setLibniControllerListener(this);
        networkMultiplexer = new NetworkMultiplexer(networkConfig.reader());
        networkMultiplexer.setLibniControllerListener(this);
        gdxControllerMultiplexer = new GdxReceiverMultiplexer(gdxReceiverMapping);
        gdxControllerMultiplexer.setLibniControllerListener(this);
    }

    public void initInputMultiplexer() {
        inputMultiplexer = new InputMultiplexer(this);
        Gdx.input.setInputProcessor(inputMultiplexer); // init multiplexed InputProcessor
        Controllers.clearListeners();
        Controllers.addListener(this);
    }

    /**
     * init a merge of keyboard input, controller input, and network input
     * if anything should be not used, specify null as its parameter
     *
     * @param gdxKeyboardMapping
     * @param newControllerCallback
     * @param networkConfig
     */
    public GdxMergedInput(GdxKeyboardMapping gdxKeyboardMapping,
                          NewNetworkReceiverCallback newControllerCallback, FileHandle networkConfig,
                          GdxReceiverMapping gdxReceiverMapping) {
        init(gdxKeyboardMapping, newControllerCallback,networkConfig, gdxReceiverMapping);
    }


    public GdxMergedInput() {
        init(null, null, null, null);
    }

    public void addInputProcessor(InputProcessor processor) {
        inputMultiplexer.addProcessor(processor);
    }

    boolean getButton(int buttonId ) {
        return mergedButtons[buttonId];
    }

    long getAnalog(int analogId ) {
        return mergedAnalogs[analogId];
    }



    int getDirections(int up, int right, int down, int left) {
        int dir = 0;
        if(getButton(left)) dir += 1;
        dir <<= 1;
        if(getButton(down)) dir += 1;
        dir <<= 1;
        if(getButton(right)) dir += 1;
        dir <<= 1;
        if(getButton(up)) dir += 1;
        return dir;
    }

    public int getDirections() {
        return getDirections(LibniMapping.BUTTON_UP, LibniMapping.BUTTON_RIGHT, LibniMapping.BUTTON_DOWN, LibniMapping.BUTTON_LEFT);
    }

    /**
     * call this from render to trigger updates
     */
    public void evaluate() {
        keyboardController.evaluate();
        networkMultiplexer.evaluate();
        gdxControllerMultiplexer.evaluate();
    }

    public void dispose() {
        keyboardController.dispose();
        networkMultiplexer.dispose();
        gdxControllerMultiplexer.dispose();
    }

    public void setListener(LibniListener listener) {
        this.listener = listener;
    }

    public void clearListener() {
        listener = null;
    }

    ////////// Libni listening methods
    @Override
    public void buttonUpdated(int button, boolean pressed) {
        // merge from three types of controllers
        boolean onePressed = keyboardController.getButton(button)
                || networkMultiplexer.getButton(button)
                || gdxControllerMultiplexer.getButton(button);
        if(mergedButtons[button] != onePressed) {
            mergedButtons[button] = onePressed;
            if(listener != null) {
                if (onePressed) listener.libniDown(button);
                else listener.libniUp(button);
            }
        }
    }

    @Override
    public void analogUpdated(int analogNr, long value) {
        long analog1 = keyboardController.getAnalog(analogNr);
        long analog1Abs = Math.abs(analog1);
        long analog2 = networkMultiplexer.getAnalog(analogNr);
        long analog2Abs = Math.abs(analog2);
        long analog3 = gdxControllerMultiplexer.getAnalog(analogNr);
        long analog3Abs = Math.abs(analog3);
        if(analog2Abs>analog1Abs) {
            analog1=analog2;
            analog1Abs=analog2Abs;
        }
        if(analog3Abs>analog1Abs)
            analog1=analog3;
        if(mergedAnalogs[analogNr] != analog1 ) {
            mergedAnalogs[analogNr] = analog1;
            if(listener!=null) {
                listener.libniAnalog(analogNr,analog1);
            }
        }
    }

    //////////// gdx input events
    @Override
    public boolean keyDown(int keycode) {
        keyboardController.registerButtonEvent(keycode);
        return false;
    }

    @Override
    public boolean keyUp(int keycode) {
        keyboardController.registerButtonEvent(keycode);
        return false;
    }

    @Override
    public boolean keyTyped(char character) {
        // don't need this as we can handle this cia keyDown and keyUp
        return false;
    }

    @Override
    public boolean touchDown(int screenX, int screenY, int pointer, int button) {
        // we don't handle mouse and touch events here
        return false;
    }

    @Override
    public boolean touchUp(int screenX, int screenY, int pointer, int button) {
        // we don't handle mouse and touch events here
        return false;
    }

    @Override
    public boolean touchDragged(int screenX, int screenY, int pointer) {
        // we don't handle mouse and touch events here
        return false;
    }

    @Override
    public boolean mouseMoved(int screenX, int screenY) {
        // we don't handle mouse and touch events here
        return false;
    }

    @Override
    public boolean scrolled(int amount) {
        // we don't handle mouse and touch events here
        return false;
    }

    ///////////// gdx controller events
    @Override
    public void connected(Controller controller) {
        // TODO: trigger controller multiplexing
        Gdx.app.log("connected","LibniReceiver: "+controller.getName());
    }

    @Override
    public void disconnected(Controller controller) {
        // TODO: trigger controller multiplexing
        Gdx.app.log("disconnected","LibniReceiver: "+controller.getName());
    }

    @Override
    public boolean buttonDown(Controller controller, int buttonCode) {
        //Gdx.app.log("buttonDown", "LibniReceiver: " + controller.getName()
        //        + " buttonCode: " + buttonCode);
        gdxControllerMultiplexer.registerButtonEvent(buttonCode + GdxReceiverMultiplexer.BUTTON1);
        return false;
    }

    @Override
    public boolean buttonUp(Controller controller, int buttonCode) {
        //Gdx.app.log("buttonUp", "LibniReceiver: " + controller.getName()
        //        + " buttonCode: " + buttonCode);
        gdxControllerMultiplexer.registerButtonEvent(buttonCode + GdxReceiverMultiplexer.BUTTON1);
        return false;
    }

    @Override
    public boolean axisMoved(Controller controller, int axisCode, float value) {
        //Gdx.app.log("axisMoved","LibniReceiver: "+controller.getName()
        //        + " axisCode: " + axisCode
        //        + " value: " + value);
        gdxControllerMultiplexer.registerAnalogEvent(axisCode);
        return false;
    }

    @Override
    public boolean povMoved(Controller controller, int povCode, PovDirection value) {
        // TODO: Handle multiple pov-codes
        //Gdx.app.log("povMoved","LibniReceiver: "+controller.getName()
        //        + " povCode: " + povCode
        //        + " PovDirection: " + value + " ordinal: " + value.ordinal());
        //if(value == PovDirection.center) { // this resets all pov-directions
        // allways update all
            for(int i=0; i<8; i++)
                gdxControllerMultiplexer.registerButtonEvent(i);
        //} else gdxControllerMultiplexer.registerButtonEvent(value.ordinal()-1);
        return false;
    }

    @Override
    public boolean xSliderMoved(Controller controller, int sliderCode, boolean value) {
        // TODO: add this as another analog?
        //Gdx.app.log("xSliderMoved","LibniReceiver: "+controller.getName()
        //        + " slidercode: " + sliderCode
        //        + " boolval: " + value);
        return false;
    }

    @Override
    public boolean ySliderMoved(Controller controller, int sliderCode, boolean value) {
        // TODO: add this as another analog?
        //Gdx.app.log("ySliderMoved","LibniReceiver: "+controller.getName()
        //        + " slidercode: " + sliderCode
        //        + " boolval: " + value);
        return false;
    }

    @Override
    public boolean accelerometerMoved(Controller controller, int accelerometerCode, Vector3 value) {
        // TODO: add this as another analog?
        //Gdx.app.log("accelerometerMoved","LibniReceiver: "+controller.getName()
        //        + " accelerometerCode: " + accelerometerCode
        //        + " Vector3: " + value);
        return false;
    }
}
