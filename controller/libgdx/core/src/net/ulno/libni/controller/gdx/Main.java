package net.ulno.libni.controller.gdx;

import com.badlogic.gdx.ApplicationAdapter;
import com.badlogic.gdx.Gdx;
import com.badlogic.gdx.Input;
import com.badlogic.gdx.InputMultiplexer;
import com.badlogic.gdx.InputProcessor;
import com.badlogic.gdx.graphics.GL20;
import com.badlogic.gdx.graphics.g2d.SpriteBatch;
import com.badlogic.gdx.scenes.scene2d.Actor;
import com.badlogic.gdx.scenes.scene2d.InputEvent;
import com.badlogic.gdx.scenes.scene2d.Stage;
import com.badlogic.gdx.scenes.scene2d.ui.*;
import com.badlogic.gdx.scenes.scene2d.utils.ChangeListener;
import com.badlogic.gdx.scenes.scene2d.utils.ClickListener;
import com.badlogic.gdx.utils.viewport.FitViewport;

import java.io.IOException;
import java.net.DatagramPacket;
import java.net.DatagramSocket;
import java.net.InetAddress;
import java.util.Random;

public class Main extends ApplicationAdapter {
    String clientId = "TST1"; // TODO save in preferences
	String destinationHost = "ulno-work";
	int destinationPort = 19877;

	public static final int NUMBER_OF_BUTTONS = 256;
	public static final int NUMBER_OF_BUTTON_BYTES = (NUMBER_OF_BUTTONS + 7)/8;
	public static final int NUMBER_OF_AXIS = 16;
	public static final int NUMBER_OF_AXIS_BYTES = NUMBER_OF_AXIS * 2;
    public static final int HEADER_LENGTH = 18;
    private byte message[] = new byte[HEADER_LENGTH + NUMBER_OF_BUTTON_BYTES + NUMBER_OF_AXIS_BYTES];
    final static byte MAGIC[] = {'L','B','N','I'};

    private Skin skin;
    private Stage stage;
    private SpriteBatch batch;


    // TODO: import these from central point -> or even switch to libgdx or wider available symbol-list
    public static final int BUTTON_UP = 16; // ctrl-p (Previous)
    public static final int BUTTON_RIGHT = 6; // ctrl-f (Forward)
    public static final int BUTTON_DOWN = 14; // ctrl-n (Next)
    public static final int BUTTON_LEFT = 2;  // ctrl-b (Back)
    public static final int BUTTON_ESCAPE = 27; // SPACE
    public static final int BUTTON_FIRE = 32; // SPACE
    public static final int BUTTON_SPACE = 32; // SPACE

	private DatagramSocket socket=null;
	DatagramPacket datagramPacket;
	private long frames=0;
    private Table rootTable;
    private Random random;

    private void setButtonState(int nr, boolean isPressed) {
		int byteNr = HEADER_LENGTH + nr / 8;
		int bitNr = nr % 8;
		if( isPressed ) {
			message[byteNr] |= 1 << bitNr;
		} else {
			message[byteNr] &= 255 - (1<<bitNr);
		}
	}

	private boolean getButtonState(int nr) {
		int byteNr = HEADER_LENGTH + nr / 8;
		int bitNr = nr % 8;
		return (message[byteNr] & (1<<bitNr))>0;
	}

	private void setAxis(int nr, int value) {
		int byteNr = HEADER_LENGTH + NUMBER_OF_BUTTONS + nr * 2;
		byte high = (byte)((value & 0xffff) >> 8);
		byte low = (byte)(value & 0xff);
		message[byteNr] = high;
		message[byteNr+1] = low;
	}

	private int getAxis(int nr) {
		int byteNr = HEADER_LENGTH + NUMBER_OF_BUTTON_BYTES + nr * 2;
		byte high = message[byteNr];
		byte low = message[byteNr+1];
		return  ((high >= 128) ? (256 - high) : high) * 256 + low; // little endian two -complement
	}

	private void send() {
		try {
			//Gdx.app.log("send","Trying to send.");
            if(datagramPacket!=null) {
                socket.send(datagramPacket);
            }
		} catch (Exception e) {
			Gdx.app.log("send","Trouble, sending.", e);
		}
	}

	@Override
	public void create() {
        random = new Random();
		batch = new SpriteBatch();
        skin = new Skin(Gdx.files.internal("skin/uiskin.json"));
//        stage = new Stage(new ScreenViewport());
        stage = new Stage(new FitViewport(600,300));

		for(int i=0; i<message.length; i++) message[i] = 0; // init
        initHeader();

        initNetwork();

		InputProcessor ip = new InputProcessor() {
			@Override
			public boolean keyDown(int keycode) {
				sendButton(true, keycode);
				return false;
			}

			@Override
			public boolean keyUp(int keycode) {
				sendButton(false, keycode);
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
		};
        Gdx.input.setInputProcessor(new InputMultiplexer(stage,ip));
        buildScreen();
	}

    private void initNetwork() {
        try {
            socket = new DatagramSocket();
            InetAddress host = InetAddress.getByName(destinationHost);
            datagramPacket = new DatagramPacket(message,message.length,host, destinationPort);
        } catch (Exception e) {
            e.printStackTrace();
            datagramPacket = null;
            socket = null;
        }
    }

    private void initHeader() {
        int pos = 0;
        for(int i=0; i<4; i++) {
            message[pos] = MAGIC[i];
            pos += 1;
        }
        message[pos++] = 0; // Protocol version = 1
        message[pos++] = 0;
        message[pos++] = 0;
        message[pos++] = 1;

        message[pos++] = 0;
        message[pos++] = 0; // Protocol type = 0 (bitblock)

        for(int i=0; i<4; i++) {
            message[pos] = (byte) random.nextInt(256);
            pos += 1;
        }
        for(int i=0; i<4; i++) {
            if(i>=clientId.length())
                message[pos]=0;
            else
                message[pos] = (byte) clientId.charAt(i);
            pos += 1;
        }
    }

    @Override
    public void resize(int width, int height) {
        /*Matrix4 matrix = new Matrix4();
        matrix.setToOrtho2D(0, 0, width, height);
        batch.setProjectionMatrix(matrix);
        stage.getBatch().setProjectionMatrix(matrix);
        stage.clear();
        buildScreen();*/
        /*rootTable.setSize(width,height);
        rootTable.pack();*/
        //rootTable.layout();
        stage.getViewport().update(width, height);
//        super.resize(width, height);
    }

    private void buildScreen() {

        rootTable = new Table();
        rootTable.setFillParent(true);
        stage.addActor(rootTable);
        rootTable.setSize(Gdx.graphics.getWidth(), Gdx.graphics.getHeight());

        rootTable.add(new Label("Libni Libgdx Controller", skin, "title")).colspan(3);
        rootTable.row();

        Table buttonsTable = new Table();
        buttonsTable.defaults().pad(2.0f);
        TextButton fireButton1 = new TextButton("Fire 1", skin, "round");
        buttonsTable.add(fireButton1);
        buttonsTable.row();
        buttonsTable.top();
        rootTable.add(buttonsTable).fill();

        final Touchpad tp = new Touchpad(0, skin);
		rootTable.add(tp).pad(5.0f).fill();

		Table inputTable = new Table();
        inputTable.defaults().pad(2.0f).top();
        TextButton fireButton2 = new TextButton("Fire 2", skin, "round");
        inputTable.add(fireButton2).colspan(2);
        inputTable.row();
        inputTable.add(new Label("Server Settings", skin, "title-plain")).colspan(2);
        inputTable.row();
        inputTable.add(new Label(" Host: ", skin, "subtitle"));
        final TextField tfHost = new TextField(destinationHost, skin);
        tfHost.setMessageText("Hostname or IP");
        tfHost.setFocusTraversal(true);
        tfHost.setTextFieldListener(new TextField.TextFieldListener() {
            @Override
            public void keyTyped(TextField textField, char c) {
                destinationHost = tfHost.getText();
                initNetwork();
            }
        });
        inputTable.add(tfHost);
        inputTable.row();
        inputTable.add(new Label(" Port: ", skin, "subtitle"));
        final TextField tfPort = new TextField(""+ destinationPort, skin);
        tfPort.setMessageText("Destination Port");
        tfPort.setFocusTraversal(true);
//        textField.setPasswordMode(true);
//        textField.setPasswordCharacter('*');
        inputTable.add(tfPort);
        tfPort.setTextFieldListener(new TextField.TextFieldListener() {
            @Override
            public void keyTyped(TextField textField, char c) {
                destinationPort = Integer.parseInt(tfPort.getText());
                initNetwork();
            }
        });
        inputTable.row();
        inputTable.add(new Label("Client Settings", skin, "title-plain")).colspan(2);
        inputTable.row();
        inputTable.add(new Label(" ID (4 Chars): ", skin, "subtitle"));
        final TextField tfClientId = new TextField(clientId, skin);
        tfClientId.setMessageText("id");
        tfClientId.setFocusTraversal(true);
        inputTable.add(tfClientId);
        tfClientId.setTextFieldListener(new TextField.TextFieldListener() {
            @Override
            public void keyTyped(TextField textField, char c) {
                clientId = tfClientId.getText();
                initHeader();
            }
        });
        inputTable.row();
        inputTable.top();
        rootTable.add(inputTable).fill();

		rootTable.row();

        /*fireButton1.addListener(new ChangeListener() {
            @Override
            public void changed(ChangeListener.ChangeEvent event, Actor actor) {
                new Dialog("Shade UI", skin, "dialog") {
                    protected void result(Object object) {
                        System.out.println("Chosen: " + object);
                    }
                }.text("Are you sure?").button("Yes", true).button("No", false)
                        .key(Input.Keys.ENTER, true).key(Input.Keys.ESCAPE, false).show(stage).getTitleLabel().setAlignment(Align.center);
            }
        });*/
        ClickListener l = new ClickListener() {
            @Override
            public boolean touchDown(InputEvent event, float x, float y, int pointer, int button) {
                setButtonState(BUTTON_FIRE, true);
                return super.touchDown(event, x, y, pointer, button);
            }

            @Override
            public void touchUp(InputEvent event, float x, float y, int pointer, int button) {
                setButtonState(BUTTON_FIRE, false);
                super.touchUp(event, x, y, pointer, button);
            }
        };
        fireButton1.addListener(l);
        fireButton2.addListener(l);
        tp.addListener(new ChangeListener() {
            @Override
            public void changed(ChangeEvent event, Actor actor) {
                float knobPercentX = tp.getKnobPercentX();
                float knobPercentY = tp.getKnobPercentY();
                int directions = 0;
                if(knobPercentX > 0.2) {
                    setButtonState(BUTTON_RIGHT, true);
                    setButtonState(BUTTON_LEFT, false);
                }
                else if(knobPercentX < -0.2) {
                    setButtonState(BUTTON_RIGHT, false);
                    setButtonState(BUTTON_LEFT, true);
                }
                else {
                    setButtonState(BUTTON_RIGHT, false);
                    setButtonState(BUTTON_LEFT, false);
                }
                if(knobPercentY > 0.2) {
                    setButtonState(BUTTON_UP, true);
                    setButtonState(BUTTON_DOWN, false);
                }
                else if(knobPercentY < -0.2) {
                    setButtonState(BUTTON_UP, false);
                    setButtonState(BUTTON_DOWN, true);
                } else {
                    setButtonState(BUTTON_UP, false);
                    setButtonState(BUTTON_DOWN, false);
                }
            }
        });
    }

	private void sendButton(boolean isPressed, int keycode) {
		char send=0;
		switch (keycode) {
			case Input.Keys.UP:
                send = BUTTON_UP;
                break;
			case Input.Keys.W:
				send = 'w';
				break;
			case Input.Keys.RIGHT:
                send = BUTTON_RIGHT;
                break;
			case Input.Keys.D:
				send = 'd';
				break;
			case Input.Keys.DOWN:
                send = BUTTON_DOWN;
                break;
            case Input.Keys.S:
				send = 's';
				break;
			case Input.Keys.LEFT:
                send = BUTTON_LEFT;
                break;
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
		if(send > 0) {
			setButtonState(send, isPressed);
			send();
		}
	}

	@Override
	public void render() {
		frames ++;
		if(frames > 3) { // every 1/20s
			frames = 0;
			send(); // send updates all the time
		}
		Gdx.gl.glClearColor(0, 0, 0, 1);
		Gdx.gl.glClear(GL20.GL_COLOR_BUFFER_BIT);
//		batch.begin();
//		batch.draw(img, 0, 0);
//		batch.end();
        stage.act(Gdx.graphics.getDeltaTime());
        stage.draw();

    }
}
