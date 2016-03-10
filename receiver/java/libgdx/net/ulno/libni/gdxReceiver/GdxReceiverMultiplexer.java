package net.ulno.libni.gdxReceiver;

import com.badlogic.gdx.controllers.Controllers;
import com.badlogic.gdx.controllers.PovDirection;
import com.badlogic.gdx.utils.Array;
import net.ulno.libni.receiver.LibniReceiver;

/**
 * Handle physical libniReceivers conencted to device handled by libgdx.LibniReceiver
 *
 * Created by ulno on 23.02.16.
 */
public class GdxReceiverMultiplexer extends LibniReceiver { // TODO: add later actual subcontrollers with creation and deletion listeners
    public static final float CONTROLLER_THRESHOLD = 0.2f; // Make this configurable per controller or even per axis
    private static final int POV_CENTER = -1;
    public static final int POV_NORTH = 0;
    public static final int POV_NORTHEAST = 4;
    public static final int POV_EAST = 2;
    public static final int POV_SOUTHEAST = 5;
    public static final int POV_SOUTH = 1;
    public static final int POV_SOUTHWEST = 7;
    public static final int POV_WEST = 3;
    public static final int POV_NORTHWEST = 6;
    public static final int BUTTON1 = 8;
    public static final int BUTTON2 = 9;
    public static final int BUTTON3 = 10;
    public static final int BUTTON4 = 11;
    public static final int BUTTON5 = 12;
    public static final int BUTTON6 = 13;
    public static final int BUTTON7 = 14;
    public static final int BUTTON8 = 15;

    public GdxReceiverMultiplexer(GdxReceiverMapping gdxReceiverMapping) {
        super(gdxReceiverMapping);
    }

    @Override
    protected boolean unmappedButtonPressed(int unmappedButtonNr) {
        boolean pressed = false;
        Array<com.badlogic.gdx.controllers.Controller> controllers = Controllers.getControllers();
        for(int i=controllers.size-1; i>=0; i--) {
            com.badlogic.gdx.controllers.Controller c = controllers.get(i);
            int povCode = 0;
            switch(unmappedButtonNr) {
                case POV_CENTER:
                    // this needs to disable all pov-buttons
                    pressed = false; // center can't be pressed
                    break;
                case POV_NORTH:
                    pressed |= c.getPov(povCode) == PovDirection.north;
                    break;
                case POV_NORTHEAST:
                    pressed |= c.getPov(povCode) == PovDirection.northEast;
                    break;
                case POV_EAST:
                    pressed |= c.getPov(povCode) == PovDirection.east;
                    break;
                case POV_SOUTHEAST:
                    pressed |= c.getPov(povCode) == PovDirection.southEast;
                    break;
                case POV_SOUTH:
                    pressed |= c.getPov(povCode) == PovDirection.south;
                    break;
                case POV_SOUTHWEST:
                    pressed |= c.getPov(povCode) == PovDirection.southWest;
                    break;
                case POV_WEST:
                    pressed |= c.getPov(povCode) == PovDirection.west;
                    break;
                case POV_NORTHWEST:
                    pressed |= c.getPov(povCode) == PovDirection.northWest;
                    break;
                case BUTTON1:
                    pressed |= c.getButton(0);
                    break;
                case BUTTON2:
                    pressed |= c.getButton(1);
                    break;
                case BUTTON3:
                    pressed |= c.getButton(2);
                    break;
                case BUTTON4:
                    pressed |= c.getButton(3);
                    break;
                case BUTTON5:
                    pressed |= c.getButton(4);
                    break;
                case BUTTON6:
                    pressed |= c.getButton(5);
                    break;
                case BUTTON7:
                    pressed |= c.getButton(6);
                    break;
                case BUTTON8:
                    pressed |= c.getButton(7);
                    break;
            }
        }

        return pressed;
    }

    @Override
    protected long unmappedAnalog(int unmappedAnalogNr) {
        Array<com.badlogic.gdx.controllers.Controller> controllers = Controllers.getControllers();
        float axisValue = 0;
        float axisValueAbs = 0;
        float currentAxis;
        float currentAxisAbs;
        for (int i = controllers.size - 1; i >= 0; i--) {
            com.badlogic.gdx.controllers.Controller c = controllers.get(i);
            currentAxis = c.getAxis(unmappedAnalogNr);
            currentAxisAbs = Math.abs(currentAxis);
            //Gdx.app.log("unmappedAnalog","Nr: "+unmappedAnalogNr+" Value: "+currentAxis);
            if (currentAxisAbs > axisValueAbs) { // select biggest absolute value on this axis
                axisValue = currentAxis;
                axisValueAbs = currentAxisAbs;
            }
        }
        if (axisValueAbs >= CONTROLLER_THRESHOLD) {
            return (long) (axisValue * 0x7fffffff);
        }
        return 0; // else
    }
}
