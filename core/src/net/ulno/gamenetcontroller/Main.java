package net.ulno.gamenetcontroller;

import com.badlogic.gdx.ApplicationAdapter;
import com.badlogic.gdx.Gdx;
import com.badlogic.gdx.Input;
import com.badlogic.gdx.InputProcessor;
import com.badlogic.gdx.graphics.GL20;
import com.badlogic.gdx.graphics.Texture;
import com.badlogic.gdx.graphics.g2d.SpriteBatch;

import org.eclipse.paho.client.mqttv3.MqttClient;
import org.eclipse.paho.client.mqttv3.MqttConnectOptions;
import org.eclipse.paho.client.mqttv3.MqttException;
import org.eclipse.paho.client.mqttv3.MqttMessage;

public class Main extends ApplicationAdapter {
    SpriteBatch batch;
    Texture img;
    public static final String mqttTopicBase = "MqttController";
    private MqttClient mqttClient;
    private String clientId;
    private final String mqttSubTopic = "McMinos"; // TODO: unhardcode this
    private String hostAndPort = "wotmqtt"; // TODO: unhardcode this
    private String mqttTopic;

    private void mqttSend(String content) {
        //System.out.println("Publishing message: " + content);
        MqttMessage message = new MqttMessage(content.getBytes());
        message.setQos(1);
        try {
            mqttClient.publish(mqttTopic, message);
            //System.out.println("Message published");
        } catch (MqttException me) {
            System.out.println("reason " + me.getReasonCode());
            System.out.println("msg " + me.getMessage());
            System.out.println("loc " + me.getLocalizedMessage());
            System.out.println("cause " + me.getCause());
            System.out.println("excep " + me);
            me.printStackTrace();
        }
    }

    @Override
    public void create() {
        batch = new SpriteBatch();
        img = new Texture("badlogic.jpg");
        // mqtt
        mqttTopic = mqttTopicBase + "/" + mqttSubTopic;
        clientId = MqttClient.generateClientId();
        try {
            mqttClient = new MqttClient("tcp://" + hostAndPort, clientId);
            MqttConnectOptions connOpts = new MqttConnectOptions();
            connOpts.setCleanSession(true);
            mqttClient.connect(connOpts);


        } catch (MqttException me) {
            System.out.println("reason " + me.getReasonCode());
            System.out.println("msg " + me.getMessage());
            System.out.println("loc " + me.getLocalizedMessage());
            System.out.println("cause " + me.getCause());
            System.out.println("excep " + me);
            me.printStackTrace();
        }

        Gdx.input.setInputProcessor(new InputProcessor() {
            @Override
            public boolean keyDown(int keycode) {
                sendButton('d', keycode);
                return false;
            }

            @Override
            public boolean keyUp(int keycode) {
                sendButton('u', keycode);
                return false;
            }

            @Override
            public boolean keyTyped(char character) {
                return false;
            }

            @Override
            public boolean touchDown(int screenX, int screenY, int pointer, int button) {
                return false;
            }

            @Override
            public boolean touchUp(int screenX, int screenY, int pointer, int button) {
                return false;
            }

            @Override
            public boolean touchDragged(int screenX, int screenY, int pointer) {
                return false;
            }

            @Override
            public boolean mouseMoved(int screenX, int screenY) {
                return false;
            }

            @Override
            public boolean scrolled(int amount) {
                return false;
            }
        });
    }

    private void sendButton(char u, int keycode) {
        char send=0;
        switch (keycode) {
            case Input.Keys.UP:
            case Input.Keys.W:
                send = 'w';
                break;
            case Input.Keys.RIGHT:
            case Input.Keys.D:
                send = 'd';
                break;
            case Input.Keys.DOWN:
            case Input.Keys.S:
                send = 's';
                break;
            case Input.Keys.LEFT:
            case Input.Keys.A:
                send = 'a';
                break;
            case Input.Keys.SPACE:
                send = ' ';
                break;
            case Input.Keys.NUM_0:
                send = '0';
                break;
            case Input.Keys.NUM_1:
                send = '1';
                break;
            case Input.Keys.NUM_2:
                send = '2';
                break;
            case Input.Keys.NUM_3:
                send = '3';
                break;
            case Input.Keys.NUM_4:
                send = '4';
                break;
            case Input.Keys.NUM_5:
                send = '5';
                break;
            case Input.Keys.NUM_6:
                send = '6';
                break;
            case Input.Keys.NUM_7:
                send = '7';
                break;
            case Input.Keys.NUM_8:
                send = '8';
                break;
            case Input.Keys.NUM_9:
                send = '9';
                break;
            case Input.Keys.ESCAPE:
                send = 27;
                break;
        }
        if(send > 0)
            mqttSend(""+u+send);
    }

    @Override
    public void render() {
        Gdx.gl.glClearColor(1, 0, 0, 1);
        Gdx.gl.glClear(GL20.GL_COLOR_BUFFER_BIT);
        batch.begin();
        batch.draw(img, 0, 0);
        batch.end();
    }
}
