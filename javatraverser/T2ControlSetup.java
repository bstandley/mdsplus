/*
		A basic implementation of the DeviceSetup class.
*/

import java.awt.*;
import javax.swing.JTabbedPane;
import javax.swing.JPanel;

public class T2ControlSetup extends DeviceSetup
{
	public T2ControlSetup(Frame parent)
	{
		super(parent);
		
		// This code is automatically generated by Visual Cafe when you add
		// components to the visual environment. It instantiates and initializes
		// the components. To modify the code, only use code syntax that matches
		// what Visual Cafe can generate, or Visual Cafe may be unable to back
		// parse your Java file into its visual environment.
		//{{INIT_CONTROLS
		setDeviceProvider("localhost");
		setDeviceTitle("Control Supervision for T2");
		setDeviceType("T2Control");
		getContentPane().setLayout(null);
		setSize(702,478);
		deviceField1.setNumCols(25);
		deviceField1.setTextOnly(true);
		deviceField1.setOffsetNid(1);
		deviceField1.setLabelString("Comment: ");
		getContentPane().add(deviceField1);
		deviceField1.setBounds(0,12,408,40);
		deviceField2.setNumCols(12);
		deviceField2.setTextOnly(true);
		deviceField2.setOffsetNid(2);
		deviceField2.setLabelString("VME IP:");
		getContentPane().add(deviceField2);
		deviceField2.setBounds(420,12,228,40);
		deviceField3.setNumCols(8);
		deviceField3.setIdentifier("frequency");
		deviceField3.setOffsetNid(3);
		deviceField3.setLabelString("Frequency (Hz): ");
		getContentPane().add(deviceField3);
		deviceField3.setBounds(0,108,228,40);
		deviceChoice1.setOffsetNid(6);
		{
			String[] tempString = new String[5];
			tempString[0] = "Offset Correction";
			tempString[1] = "Intelligent Shell";
			tempString[2] = "Rotating Perturbation";
			tempString[3] = "Mode Control";
			tempString[4] = "Individual Perturbation";
			deviceChoice1.setChoiceItems(tempString);
		}
		deviceChoice1.setLabelString("Initial Control");
		deviceChoice1.setConvert(true);
		deviceChoice1.setChoiceIntValues(new int[] {});
		getContentPane().add(deviceChoice1);
		deviceChoice1.setBounds(12,156,276,40);
		deviceChoice2.setOffsetNid(7);
		{
			String[] tempString = new String[5];
			tempString[0] = "Offset Correction";
			tempString[1] = "Intelligent Shell";
			tempString[2] = "Rotating Perturbation";
			tempString[3] = "Mode Control";
			tempString[4] = "Individual Perturbation";
			deviceChoice2.setChoiceItems(tempString);
		}
		deviceChoice2.setLabelString("Triggered Control");
		deviceChoice2.setConvert(true);
		deviceChoice2.setChoiceIntValues(new int[] {});
		getContentPane().add(deviceChoice2);
		deviceChoice2.setBounds(312,156,300,40);
		deviceField4.setNumCols(4);
		deviceField4.setIdentifier("contr_durat");
		deviceField4.setOffsetNid(9);
		deviceField4.setLabelString("Control duration (s):");
		getContentPane().add(deviceField4);
		deviceField4.setBounds(228,108,216,40);
		deviceTable1.setNumCols(2);
		deviceTable1.setDisplayRowNumber(true);
		{
			String[] tempString = new String[2];
			tempString[0] = "Gain";
			tempString[1] = "Offset";
			deviceTable1.setColumnNames(tempString);
		}
		deviceTable1.setNumRows(64);
		deviceTable1.setOffsetNid(4);
		deviceTable1.setLabelString("In Calibration");
		getContentPane().add(deviceTable1);
		deviceTable1.setBounds(12,204,192,108);
		JTabbedPane1.setAlignmentX(7.0F);
		getContentPane().add(JTabbedPane1);
		JTabbedPane1.setBounds(216,216,480,192);
		JPanel1.setLayout(new FlowLayout(FlowLayout.CENTER,5,5));
		JTabbedPane1.add(JPanel1);
		JPanel1.setBounds(2,24,475,165);
		deviceField5.setNumCols(5);
		deviceField5.setOffsetNid(14);
		deviceField5.setLabelString("Kp:");
		JPanel1.add(deviceField5);
		deviceField6.setNumCols(5);
		deviceField6.setOffsetNid(16);
		deviceField6.setLabelString("Ki:");
		JPanel1.add(deviceField6);
		deviceField48.setNumCols(5);
		deviceField48.setOffsetNid(18);
		deviceField48.setLabelString("Kd:");
		JPanel1.add(deviceField48);
		JPanel2.setLayout(new FlowLayout(FlowLayout.CENTER,5,5));
		JTabbedPane1.add(JPanel2);
		JPanel2.setBounds(2,24,475,165);
		deviceField8.setNumCols(2);
		deviceField8.setIdentifier("n1");
		deviceField8.setOffsetNid(20);
		deviceField8.setLabelString("N 1:");
		JPanel2.add(deviceField8);
		deviceField9.setNumCols(4);
		deviceField9.setOffsetNid(22);
		deviceField9.setLabelString("Amp. : ");
		JPanel2.add(deviceField9);
		deviceField10.setNumCols(4);
		deviceField10.setOffsetNid(22);
		deviceField10.setLabelString("Freq. (Hz)");
		JPanel2.add(deviceField10);
		deviceField11.setNumCols(4);
		deviceField11.setOffsetNid(26);
		deviceField11.setLabelString("Phase (Rad):");
		JPanel2.add(deviceField11);
		deviceField36.setNumCols(2);
		deviceField36.setIdentifier("n2");
		deviceField36.setOffsetNid(28);
		deviceField36.setLabelString("N 2:");
		JPanel2.add(deviceField36);
		deviceField37.setNumCols(4);
		deviceField37.setOffsetNid(30);
		deviceField37.setLabelString("Amp. : ");
		JPanel2.add(deviceField37);
		deviceField38.setNumCols(4);
		deviceField38.setOffsetNid(32);
		deviceField38.setLabelString("Freq. (Hz)");
		JPanel2.add(deviceField38);
		deviceField39.setNumCols(4);
		deviceField39.setOffsetNid(34);
		deviceField39.setLabelString("Phase (Rad):");
		JPanel2.add(deviceField39);
		deviceField40.setNumCols(2);
		deviceField40.setIdentifier("n3");
		deviceField40.setOffsetNid(36);
		deviceField40.setLabelString("N 3:");
		JPanel2.add(deviceField40);
		deviceField41.setNumCols(4);
		deviceField41.setOffsetNid(38);
		deviceField41.setLabelString("Amp. : ");
		JPanel2.add(deviceField41);
		deviceField42.setNumCols(4);
		deviceField42.setOffsetNid(40);
		deviceField42.setLabelString("Freq. (Hz)");
		JPanel2.add(deviceField42);
		deviceField43.setNumCols(4);
		deviceField43.setOffsetNid(42);
		deviceField43.setLabelString("Phase (Rad):");
		JPanel2.add(deviceField43);
		deviceField44.setNumCols(2);
		deviceField44.setIdentifier("n4");
		deviceField44.setOffsetNid(44);
		deviceField44.setLabelString("N 4:");
		JPanel2.add(deviceField44);
		deviceField45.setNumCols(4);
		deviceField45.setOffsetNid(46);
		deviceField45.setLabelString("Amp. : ");
		JPanel2.add(deviceField45);
		deviceField46.setNumCols(4);
		deviceField46.setOffsetNid(48);
		deviceField46.setLabelString("Freq. (Hz)");
		JPanel2.add(deviceField46);
		deviceField47.setNumCols(4);
		deviceField47.setOffsetNid(50);
		deviceField47.setLabelString("Phase (Rad):");
		JPanel2.add(deviceField47);
		JPanel3.setLayout(new GridLayout(3,2,0,0));
		JTabbedPane1.add(JPanel3);
		deviceField12.setNumCols(8);
		deviceField12.setOffsetNid(52);
		deviceField12.setLabelString("K1:");
		JPanel3.add(deviceField12);
		deviceField13.setNumCols(8);
		deviceField13.setOffsetNid(54);
		deviceField13.setLabelString("K2:");
		JPanel3.add(deviceField13);
		deviceField14.setNumCols(8);
		deviceField14.setOffsetNid(56);
		deviceField14.setLabelString("K3:");
		JPanel3.add(deviceField14);
		deviceField15.setNumCols(8);
		deviceField15.setOffsetNid(58);
		deviceField15.setLabelString("K4:");
		JPanel3.add(deviceField15);
		JPanel4.setLayout(new GridLayout(4,4,0,0));
		JTabbedPane1.add(JPanel4);
		JPanel4.setBounds(2,24,475,165);
		deviceField17.setNumCols(2);
		deviceField17.setIdentifier("id1");
		deviceField17.setOffsetNid(62);
		deviceField17.setLabelString("Out id:");
		JPanel4.add(deviceField17);
		deviceField18.setNumCols(4);
		deviceField18.setOffsetNid(64);
		deviceField18.setLabelString("Amp.:");
		JPanel4.add(deviceField18);
		deviceField19.setNumCols(4);
		deviceField19.setOffsetNid(66);
		deviceField19.setLabelString("Omega:");
		JPanel4.add(deviceField19);
		deviceField20.setNumCols(4);
		deviceField20.setOffsetNid(68);
		deviceField20.setLabelString("Phi:");
		JPanel4.add(deviceField20);
		deviceField21.setNumCols(2);
		deviceField21.setIdentifier("id2");
		deviceField21.setOffsetNid(70);
		deviceField21.setLabelString("Out id:");
		JPanel4.add(deviceField21);
		deviceField22.setNumCols(4);
		deviceField22.setOffsetNid(72);
		deviceField22.setLabelString("Amp.:");
		JPanel4.add(deviceField22);
		deviceField23.setNumCols(4);
		deviceField23.setOffsetNid(74);
		deviceField23.setLabelString("Omega:");
		JPanel4.add(deviceField23);
		deviceField24.setNumCols(4);
		deviceField24.setOffsetNid(76);
		deviceField24.setLabelString("Phi:");
		JPanel4.add(deviceField24);
		deviceField25.setNumCols(2);
		deviceField25.setIdentifier("id3");
		deviceField25.setOffsetNid(78);
		deviceField25.setLabelString("Out id:");
		JPanel4.add(deviceField25);
		deviceField26.setNumCols(4);
		deviceField26.setOffsetNid(80);
		deviceField26.setLabelString("Amp.:");
		JPanel4.add(deviceField26);
		deviceField27.setNumCols(4);
		deviceField27.setOffsetNid(82);
		deviceField27.setLabelString("Omega:");
		JPanel4.add(deviceField27);
		deviceField28.setNumCols(4);
		deviceField28.setOffsetNid(84);
		deviceField28.setLabelString("Phi:");
		JPanel4.add(deviceField28);
		deviceField29.setNumCols(2);
		deviceField29.setIdentifier("id4");
		deviceField29.setOffsetNid(86);
		deviceField29.setLabelString("Out id:");
		JPanel4.add(deviceField29);
		deviceField30.setNumCols(4);
		deviceField30.setOffsetNid(88);
		deviceField30.setLabelString("Amp.:");
		JPanel4.add(deviceField30);
		deviceField31.setNumCols(4);
		deviceField31.setOffsetNid(90);
		deviceField31.setLabelString("Omega:");
		JPanel4.add(deviceField31);
		deviceField32.setNumCols(4);
		deviceField32.setOffsetNid(92);
		deviceField32.setLabelString("Phi:");
		JPanel4.add(deviceField32);
		JPanel3.setBounds(2,24,475,165);
		JTabbedPane1.setTitleAt(0,"Intelligent Shell");
		JTabbedPane1.setTitleAt(1,"Rot. Perturbation");
		JTabbedPane1.setTitleAt(2,"Mode Control");
		JTabbedPane1.setTitleAt(3,"Ind.Perturbation");
		{
			String[] tempString = new String[8];
			tempString[0] = "N1 must be between 1 and 32";
			tempString[1] = "N2 must be between 1 and 32";
			tempString[2] = "N4 must be between 1 and 32";
			tempString[3] = "N3 must be between 1 and 32";
			tempString[4] = "Id 1 must be between 1 and 16";
			tempString[5] = "Id 2 must be between 1 and 16";
			tempString[6] = "Id 3 must be between 1 and 16";
			tempString[7] = "Id 4 must be between 1 and 16";
			deviceButtons1.setCheckMessages(tempString);
		}
		{
			String[] tempString = new String[8];
			tempString[0] = "_n1 > 0 && _n1 < 33";
			tempString[1] = "_n2 > 0 && _n2 < 33";
			tempString[2] = "_n4 > 0 && _n4< 33";
			tempString[3] = "_n3 > 0 && _n3< 33";
			tempString[4] = "_id1 > 0 && _id1 < 16";
			tempString[5] = "_id2 > 0 && _id2 < 16";
			tempString[6] = "_id3 > 0 && _id3 < 16";
			tempString[7] = "_id4 > 0 && _id4 < 16";
			deviceButtons1.setCheckExpressions(tempString);
		}
		{
			String[] tempString = new String[3];
			tempString[0] = "INIT";
			tempString[1] = "ABORT";
			tempString[2] = "STORE";
			deviceButtons1.setMethods(tempString);
		}
		getContentPane().add(deviceButtons1);
		deviceButtons1.setBounds(144,432,336,40);
		deviceField16.setNumCols(5);
		deviceField16.setOffsetNid(8);
		deviceField16.setLabelString("Trigger time (s):");
		getContentPane().add(deviceField16);
		deviceField16.setBounds(0,60,204,40);
		deviceField33.setNumCols(5);
		deviceField33.setIdentifier("pre_time");
		deviceField33.setOffsetNid(11);
		deviceField33.setLabelString("Presampling time (s):");
		getContentPane().add(deviceField33);
		deviceField33.setBounds(204,60,204,40);
		deviceField34.setNumCols(5);
		deviceField34.setIdentifier("post_time");
		deviceField34.setOffsetNid(12);
		deviceField34.setLabelString("Postsampling time (s):");
		getContentPane().add(deviceField34);
		deviceField34.setBounds(408,60,240,40);
		deviceField35.setNumCols(5);
		deviceField35.setIdentifier("sys_durat");
		deviceField35.setOffsetNid(10);
		deviceField35.setLabelString("System duration (s):");
		getContentPane().add(deviceField35);
		deviceField35.setBounds(432,108,228,40);
		deviceTable2.setNumCols(2);
		deviceTable2.setDisplayRowNumber(true);
		{
			String[] tempString = new String[2];
			tempString[0] = "Gain";
			tempString[1] = "Offset";
			deviceTable2.setColumnNames(tempString);
		}
		deviceTable2.setNumRows(32);
		deviceTable2.setOffsetNid(5);
		deviceTable2.setLabelString("Out Calibration");
		getContentPane().add(deviceTable2);
		deviceTable2.setBounds(12,312,192,108);
		//}}
	}

	public T2ControlSetup()
	{
		this((Frame)null);
	}

	public T2ControlSetup(String sTitle)
	{
		this();
		setTitle(sTitle);
	}

	static public void main(String args[])
	{
		(new T2ControlSetup()).setVisible(true);
	}

	public void addNotify()
	{
		// Record the size of the window prior to calling parents addNotify.
		Dimension size = getSize();

		super.addNotify();

		if (frameSizeAdjusted)
			return;
		frameSizeAdjusted = true;

		// Adjust size of frame according to the insets
		Insets insets = getInsets();
		setSize(insets.left + insets.right + size.width, insets.top + insets.bottom + size.height);
	}

	// Used by addNotify
	boolean frameSizeAdjusted = false;

	//{{DECLARE_CONTROLS
	DeviceField deviceField1 = new DeviceField();
	DeviceField deviceField2 = new DeviceField();
	DeviceField deviceField3 = new DeviceField();
	DeviceChoice deviceChoice1 = new DeviceChoice();
	DeviceChoice deviceChoice2 = new DeviceChoice();
	DeviceField deviceField4 = new DeviceField();
	DeviceTable deviceTable1 = new DeviceTable();
	javax.swing.JTabbedPane JTabbedPane1 = new javax.swing.JTabbedPane();
	javax.swing.JPanel JPanel1 = new javax.swing.JPanel();
	DeviceField deviceField5 = new DeviceField();
	DeviceField deviceField6 = new DeviceField();
	DeviceField deviceField48 = new DeviceField();
	javax.swing.JPanel JPanel2 = new javax.swing.JPanel();
	DeviceField deviceField8 = new DeviceField();
	DeviceField deviceField9 = new DeviceField();
	DeviceField deviceField10 = new DeviceField();
	DeviceField deviceField11 = new DeviceField();
	DeviceField deviceField36 = new DeviceField();
	DeviceField deviceField37 = new DeviceField();
	DeviceField deviceField38 = new DeviceField();
	DeviceField deviceField39 = new DeviceField();
	DeviceField deviceField40 = new DeviceField();
	DeviceField deviceField41 = new DeviceField();
	DeviceField deviceField42 = new DeviceField();
	DeviceField deviceField43 = new DeviceField();
	DeviceField deviceField44 = new DeviceField();
	DeviceField deviceField45 = new DeviceField();
	DeviceField deviceField46 = new DeviceField();
	DeviceField deviceField47 = new DeviceField();
	javax.swing.JPanel JPanel3 = new javax.swing.JPanel();
	DeviceField deviceField12 = new DeviceField();
	DeviceField deviceField13 = new DeviceField();
	DeviceField deviceField14 = new DeviceField();
	DeviceField deviceField15 = new DeviceField();
	javax.swing.JPanel JPanel4 = new javax.swing.JPanel();
	DeviceField deviceField17 = new DeviceField();
	DeviceField deviceField18 = new DeviceField();
	DeviceField deviceField19 = new DeviceField();
	DeviceField deviceField20 = new DeviceField();
	DeviceField deviceField21 = new DeviceField();
	DeviceField deviceField22 = new DeviceField();
	DeviceField deviceField23 = new DeviceField();
	DeviceField deviceField24 = new DeviceField();
	DeviceField deviceField25 = new DeviceField();
	DeviceField deviceField26 = new DeviceField();
	DeviceField deviceField27 = new DeviceField();
	DeviceField deviceField28 = new DeviceField();
	DeviceField deviceField29 = new DeviceField();
	DeviceField deviceField30 = new DeviceField();
	DeviceField deviceField31 = new DeviceField();
	DeviceField deviceField32 = new DeviceField();
	DeviceButtons deviceButtons1 = new DeviceButtons();
	DeviceField deviceField16 = new DeviceField();
	DeviceField deviceField33 = new DeviceField();
	DeviceField deviceField34 = new DeviceField();
	DeviceField deviceField35 = new DeviceField();
	DeviceTable deviceTable2 = new DeviceTable();
	//}}

}