using System;
using System.Drawing;
using System.Collections;
using System.IO;
using System.ComponentModel;
using System.Windows.Forms;
using System.Data;

namespace SVCaseGen
{
	/// <summary>
	/// Summary description for Form1.
	/// </summary>
	public class SVCaseGenForm : System.Windows.Forms.Form
	{
		/// <summary>
		/// Required designer variable.
		/// </summary>
		private System.ComponentModel.Container components = null;
		private System.Windows.Forms.GroupBox groupBox1;
		private System.Windows.Forms.Label label2;
		private System.Windows.Forms.Button Generate;
		private System.Windows.Forms.Button Exit;
		private System.Windows.Forms.Label label1;
		private System.Windows.Forms.Label label3;
		private System.Windows.Forms.Label label4;
		private System.Windows.Forms.Label label5;
		private System.Windows.Forms.Label label6;
		private System.Windows.Forms.Label label7;
		private System.Windows.Forms.Label label8;
		private System.Windows.Forms.ComboBox versionList;
		private System.Windows.Forms.TextBox instructionBox;
		private System.Windows.Forms.CheckedListBox destCombo;
		private System.Windows.Forms.ComboBox sourceRegList;
		private System.Windows.Forms.CheckedListBox destModCombo;
		private System.Windows.Forms.CheckedListBox writeMaskCombo;
		private System.Windows.Forms.CheckedListBox sourceCombo1;
		private System.Windows.Forms.CheckBox srcModChk1;
		private System.Windows.Forms.CheckBox repSwizChk1;
		private System.Windows.Forms.CheckBox arbSwizChk1;
		private System.Windows.Forms.CheckBox initChk1;
		private System.Windows.Forms.CheckBox genSwizChk1;
		private System.Windows.Forms.CheckBox initChk2;
		private System.Windows.Forms.CheckBox genSwizChk2;
		private System.Windows.Forms.CheckBox arbSwizChk2;
		private System.Windows.Forms.CheckBox repSwizChk2;
		private System.Windows.Forms.CheckBox srcModChk2;
		private System.Windows.Forms.CheckedListBox sourceCombo2;
		private System.Windows.Forms.CheckBox initChk3;
		private System.Windows.Forms.CheckBox genSwizChk3;
		private System.Windows.Forms.CheckBox arbSwizChk3;
		private System.Windows.Forms.CheckBox repSwizChk3;
		private System.Windows.Forms.CheckBox srcModChk3;
		private System.Windows.Forms.CheckedListBox sourceCombo3;
		private System.Windows.Forms.GroupBox destGroup;
		private System.Windows.Forms.GroupBox srcGroup1;
		private System.Windows.Forms.GroupBox srcGroup2;
		private System.Windows.Forms.GroupBox srcGroup3;
		
		private SVCaseGen caseGen = null;
		private CheckedListBox [ ] sourceCombo;
		private CheckBox [ ] srcModChk;
		private CheckBox [ ] repSwizChk;
		private CheckBox [ ] arbSwizChk;
		private CheckBox [ ] genSwizChk;
		private CheckBox [ ] initChk;
		private System.Windows.Forms.StatusBar statusBar;
		private System.Windows.Forms.Button genFileBtn;
		private System.Windows.Forms.CheckBox staticChk;
		private System.Windows.Forms.CheckBox dynamicChk;
		private System.Windows.Forms.CheckBox allowedInDFCChk;
		private System.Windows.Forms.Button clearBtn;
		
		private string OUTPUT_PATH = System.Environment.CurrentDirectory + "\\";		


		public SVCaseGenForm()
		{
			//
			// Required for Windows Form Designer support
			//
			InitializeComponent();
			
			if( OUTPUT_PATH != "" && !Directory.Exists( OUTPUT_PATH ) )
				Directory.CreateDirectory( OUTPUT_PATH );
		
			sourceCombo	= new CheckedListBox[ ] { sourceCombo1, sourceCombo2, sourceCombo3 };
			srcModChk	= new CheckBox[ ] { srcModChk1, srcModChk2, srcModChk3 };
			repSwizChk	= new CheckBox[ ] { repSwizChk1, repSwizChk2, repSwizChk3 };
			arbSwizChk	= new CheckBox[ ] { arbSwizChk1, arbSwizChk2, arbSwizChk3 };
			genSwizChk	= new CheckBox[ ] { genSwizChk1, genSwizChk2, genSwizChk3 };
			initChk		= new CheckBox[ ] { initChk1, initChk2, initChk3 };
			caseGen = new SVCaseGen( );
			versionList.Items.AddRange( caseGen.GetShaderVersions( ) );
			versionList.SelectedIndex = 0;
		}

		/// <summary>
		/// Clean up any resources being used.
		/// </summary>
		protected override void Dispose( bool disposing )
		{
			if( disposing )
			{
				if (components != null) 
				{
					components.Dispose();
				}
			}
			base.Dispose( disposing );
		}

		#region Windows Form Designer generated code
		/// <summary>
		/// Required method for Designer support - do not modify
		/// the contents of this method with the code editor.
		/// </summary>
		private void InitializeComponent()
		{
			this.groupBox1 = new System.Windows.Forms.GroupBox();
			this.versionList = new System.Windows.Forms.ComboBox();
			this.instructionBox = new System.Windows.Forms.TextBox();
			this.label2 = new System.Windows.Forms.Label();
			this.Generate = new System.Windows.Forms.Button();
			this.Exit = new System.Windows.Forms.Button();
			this.destCombo = new System.Windows.Forms.CheckedListBox();
			this.label1 = new System.Windows.Forms.Label();
			this.sourceRegList = new System.Windows.Forms.ComboBox();
			this.destGroup = new System.Windows.Forms.GroupBox();
			this.label5 = new System.Windows.Forms.Label();
			this.writeMaskCombo = new System.Windows.Forms.CheckedListBox();
			this.label4 = new System.Windows.Forms.Label();
			this.destModCombo = new System.Windows.Forms.CheckedListBox();
			this.label3 = new System.Windows.Forms.Label();
			this.srcGroup1 = new System.Windows.Forms.GroupBox();
			this.initChk1 = new System.Windows.Forms.CheckBox();
			this.genSwizChk1 = new System.Windows.Forms.CheckBox();
			this.arbSwizChk1 = new System.Windows.Forms.CheckBox();
			this.repSwizChk1 = new System.Windows.Forms.CheckBox();
			this.srcModChk1 = new System.Windows.Forms.CheckBox();
			this.label6 = new System.Windows.Forms.Label();
			this.sourceCombo1 = new System.Windows.Forms.CheckedListBox();
			this.srcGroup2 = new System.Windows.Forms.GroupBox();
			this.initChk2 = new System.Windows.Forms.CheckBox();
			this.genSwizChk2 = new System.Windows.Forms.CheckBox();
			this.arbSwizChk2 = new System.Windows.Forms.CheckBox();
			this.repSwizChk2 = new System.Windows.Forms.CheckBox();
			this.srcModChk2 = new System.Windows.Forms.CheckBox();
			this.label7 = new System.Windows.Forms.Label();
			this.sourceCombo2 = new System.Windows.Forms.CheckedListBox();
			this.srcGroup3 = new System.Windows.Forms.GroupBox();
			this.initChk3 = new System.Windows.Forms.CheckBox();
			this.genSwizChk3 = new System.Windows.Forms.CheckBox();
			this.arbSwizChk3 = new System.Windows.Forms.CheckBox();
			this.repSwizChk3 = new System.Windows.Forms.CheckBox();
			this.srcModChk3 = new System.Windows.Forms.CheckBox();
			this.label8 = new System.Windows.Forms.Label();
			this.sourceCombo3 = new System.Windows.Forms.CheckedListBox();
			this.statusBar = new System.Windows.Forms.StatusBar();
			this.genFileBtn = new System.Windows.Forms.Button();
			this.staticChk = new System.Windows.Forms.CheckBox();
			this.dynamicChk = new System.Windows.Forms.CheckBox();
			this.allowedInDFCChk = new System.Windows.Forms.CheckBox();
			this.clearBtn = new System.Windows.Forms.Button();
			this.groupBox1.SuspendLayout();
			this.destGroup.SuspendLayout();
			this.srcGroup1.SuspendLayout();
			this.srcGroup2.SuspendLayout();
			this.srcGroup3.SuspendLayout();
			this.SuspendLayout();
			// 
			// groupBox1
			// 
			this.groupBox1.Controls.Add(this.versionList);
			this.groupBox1.Location = new System.Drawing.Point(16, 40);
			this.groupBox1.Name = "groupBox1";
			this.groupBox1.Size = new System.Drawing.Size(104, 64);
			this.groupBox1.TabIndex = 0;
			this.groupBox1.TabStop = false;
			this.groupBox1.Text = "Version";
			// 
			// versionList
			// 
			this.versionList.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
			this.versionList.Location = new System.Drawing.Point(16, 24);
			this.versionList.Name = "versionList";
			this.versionList.Size = new System.Drawing.Size(72, 21);
			this.versionList.TabIndex = 0;
			this.versionList.SelectedIndexChanged += new System.EventHandler(this.versionList_SelectedIndexChanged);
			// 
			// instructionBox
			// 
			this.instructionBox.Location = new System.Drawing.Point(232, 24);
			this.instructionBox.Name = "instructionBox";
			this.instructionBox.Size = new System.Drawing.Size(72, 20);
			this.instructionBox.TabIndex = 3;
			this.instructionBox.Text = "";
			// 
			// label2
			// 
			this.label2.Location = new System.Drawing.Point(144, 24);
			this.label2.Name = "label2";
			this.label2.Size = new System.Drawing.Size(80, 16);
			this.label2.TabIndex = 5;
			this.label2.Text = "Instruction:";
			this.label2.TextAlign = System.Drawing.ContentAlignment.MiddleRight;
			// 
			// Generate
			// 
			this.Generate.Location = new System.Drawing.Point(184, 616);
			this.Generate.Name = "Generate";
			this.Generate.TabIndex = 6;
			this.Generate.Text = "Generate...";
			this.Generate.Click += new System.EventHandler(this.Generate_Click);
			// 
			// Exit
			// 
			this.Exit.Location = new System.Drawing.Point(312, 656);
			this.Exit.Name = "Exit";
			this.Exit.TabIndex = 7;
			this.Exit.Text = "Exit";
			this.Exit.Click += new System.EventHandler(this.Exit_Click);
			// 
			// destCombo
			// 
			this.destCombo.Location = new System.Drawing.Point(16, 24);
			this.destCombo.Name = "destCombo";
			this.destCombo.Size = new System.Drawing.Size(80, 64);
			this.destCombo.TabIndex = 13;
			// 
			// label1
			// 
			this.label1.Location = new System.Drawing.Point(136, 48);
			this.label1.Name = "label1";
			this.label1.Size = new System.Drawing.Size(96, 23);
			this.label1.TabIndex = 15;
			this.label1.Text = "Source Registers:";
			// 
			// sourceRegList
			// 
			this.sourceRegList.Items.AddRange(new object[] {
															   "0",
															   "1",
															   "2",
															   "3"});
			this.sourceRegList.Location = new System.Drawing.Point(232, 48);
			this.sourceRegList.Name = "sourceRegList";
			this.sourceRegList.Size = new System.Drawing.Size(48, 21);
			this.sourceRegList.TabIndex = 16;
			this.sourceRegList.Text = "0";
			this.sourceRegList.SelectedIndexChanged += new System.EventHandler(this.sourceRegList_SelectedIndexChanged);
			// 
			// destGroup
			// 
			this.destGroup.Controls.Add(this.label5);
			this.destGroup.Controls.Add(this.writeMaskCombo);
			this.destGroup.Controls.Add(this.label4);
			this.destGroup.Controls.Add(this.destCombo);
			this.destGroup.Controls.Add(this.destModCombo);
			this.destGroup.Location = new System.Drawing.Point(16, 160);
			this.destGroup.Name = "destGroup";
			this.destGroup.Size = new System.Drawing.Size(368, 100);
			this.destGroup.TabIndex = 17;
			this.destGroup.TabStop = false;
			this.destGroup.Text = "Destination";
			// 
			// label5
			// 
			this.label5.Location = new System.Drawing.Point(240, 24);
			this.label5.Name = "label5";
			this.label5.Size = new System.Drawing.Size(112, 16);
			this.label5.TabIndex = 22;
			this.label5.Text = "Write Masks Allowed:";
			// 
			// writeMaskCombo
			// 
			this.writeMaskCombo.Location = new System.Drawing.Point(240, 40);
			this.writeMaskCombo.Name = "writeMaskCombo";
			this.writeMaskCombo.Size = new System.Drawing.Size(104, 49);
			this.writeMaskCombo.TabIndex = 21;
			// 
			// label4
			// 
			this.label4.Location = new System.Drawing.Point(120, 24);
			this.label4.Name = "label4";
			this.label4.Size = new System.Drawing.Size(104, 16);
			this.label4.TabIndex = 20;
			this.label4.Text = "Dest Mod Allowed:";
			// 
			// destModCombo
			// 
			this.destModCombo.Location = new System.Drawing.Point(120, 40);
			this.destModCombo.Name = "destModCombo";
			this.destModCombo.Size = new System.Drawing.Size(96, 49);
			this.destModCombo.TabIndex = 19;
			// 
			// label3
			// 
			this.label3.Font = new System.Drawing.Font("Microsoft Sans Serif", 10F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((System.Byte)(0)));
			this.label3.Location = new System.Drawing.Point(160, 136);
			this.label3.Name = "label3";
			this.label3.Size = new System.Drawing.Size(88, 16);
			this.label3.TabIndex = 18;
			this.label3.Text = "Registers";
			// 
			// srcGroup1
			// 
			this.srcGroup1.Controls.Add(this.initChk1);
			this.srcGroup1.Controls.Add(this.genSwizChk1);
			this.srcGroup1.Controls.Add(this.arbSwizChk1);
			this.srcGroup1.Controls.Add(this.repSwizChk1);
			this.srcGroup1.Controls.Add(this.srcModChk1);
			this.srcGroup1.Controls.Add(this.label6);
			this.srcGroup1.Controls.Add(this.sourceCombo1);
			this.srcGroup1.Enabled = false;
			this.srcGroup1.Location = new System.Drawing.Point(16, 272);
			this.srcGroup1.Name = "srcGroup1";
			this.srcGroup1.Size = new System.Drawing.Size(368, 100);
			this.srcGroup1.TabIndex = 19;
			this.srcGroup1.TabStop = false;
			this.srcGroup1.Text = "Source 1";
			// 
			// initChk1
			// 
			this.initChk1.Location = new System.Drawing.Point(240, 56);
			this.initChk1.Name = "initChk1";
			this.initChk1.Size = new System.Drawing.Size(96, 16);
			this.initChk1.TabIndex = 19;
			this.initChk1.Text = "Must Initialize";
			// 
			// genSwizChk1
			// 
			this.genSwizChk1.Location = new System.Drawing.Point(112, 34);
			this.genSwizChk1.Name = "genSwizChk1";
			this.genSwizChk1.Size = new System.Drawing.Size(112, 16);
			this.genSwizChk1.TabIndex = 18;
			this.genSwizChk1.Text = "General Swizzles";
			// 
			// arbSwizChk1
			// 
			this.arbSwizChk1.Location = new System.Drawing.Point(112, 64);
			this.arbSwizChk1.Name = "arbSwizChk1";
			this.arbSwizChk1.Size = new System.Drawing.Size(128, 16);
			this.arbSwizChk1.TabIndex = 17;
			this.arbSwizChk1.Text = "Arbitrary Swizzles";
			// 
			// repSwizChk1
			// 
			this.repSwizChk1.Location = new System.Drawing.Point(112, 48);
			this.repSwizChk1.Name = "repSwizChk1";
			this.repSwizChk1.Size = new System.Drawing.Size(120, 16);
			this.repSwizChk1.TabIndex = 16;
			this.repSwizChk1.Text = "Replicate Swizzles";
			// 
			// srcModChk1
			// 
			this.srcModChk1.Location = new System.Drawing.Point(240, 40);
			this.srcModChk1.Name = "srcModChk1";
			this.srcModChk1.Size = new System.Drawing.Size(120, 16);
			this.srcModChk1.TabIndex = 15;
			this.srcModChk1.Text = "Source Modifiers";
			// 
			// label6
			// 
			this.label6.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((System.Byte)(0)));
			this.label6.Location = new System.Drawing.Point(200, 16);
			this.label6.Name = "label6";
			this.label6.Size = new System.Drawing.Size(96, 16);
			this.label6.TabIndex = 14;
			this.label6.Text = "Allow:";
			// 
			// sourceCombo1
			// 
			this.sourceCombo1.Location = new System.Drawing.Point(16, 24);
			this.sourceCombo1.Name = "sourceCombo1";
			this.sourceCombo1.Size = new System.Drawing.Size(80, 64);
			this.sourceCombo1.TabIndex = 13;
			// 
			// srcGroup2
			// 
			this.srcGroup2.Controls.Add(this.initChk2);
			this.srcGroup2.Controls.Add(this.genSwizChk2);
			this.srcGroup2.Controls.Add(this.arbSwizChk2);
			this.srcGroup2.Controls.Add(this.repSwizChk2);
			this.srcGroup2.Controls.Add(this.srcModChk2);
			this.srcGroup2.Controls.Add(this.label7);
			this.srcGroup2.Controls.Add(this.sourceCombo2);
			this.srcGroup2.Enabled = false;
			this.srcGroup2.Location = new System.Drawing.Point(16, 384);
			this.srcGroup2.Name = "srcGroup2";
			this.srcGroup2.Size = new System.Drawing.Size(368, 100);
			this.srcGroup2.TabIndex = 20;
			this.srcGroup2.TabStop = false;
			this.srcGroup2.Text = "Source 2";
			// 
			// initChk2
			// 
			this.initChk2.Location = new System.Drawing.Point(240, 56);
			this.initChk2.Name = "initChk2";
			this.initChk2.Size = new System.Drawing.Size(96, 16);
			this.initChk2.TabIndex = 19;
			this.initChk2.Text = "Must Initialize";
			// 
			// genSwizChk2
			// 
			this.genSwizChk2.Location = new System.Drawing.Point(112, 32);
			this.genSwizChk2.Name = "genSwizChk2";
			this.genSwizChk2.Size = new System.Drawing.Size(112, 16);
			this.genSwizChk2.TabIndex = 18;
			this.genSwizChk2.Text = "General Swizzles";
			// 
			// arbSwizChk2
			// 
			this.arbSwizChk2.Location = new System.Drawing.Point(112, 64);
			this.arbSwizChk2.Name = "arbSwizChk2";
			this.arbSwizChk2.Size = new System.Drawing.Size(128, 16);
			this.arbSwizChk2.TabIndex = 17;
			this.arbSwizChk2.Text = "Arbitrary Swizzles";
			// 
			// repSwizChk2
			// 
			this.repSwizChk2.Location = new System.Drawing.Point(112, 48);
			this.repSwizChk2.Name = "repSwizChk2";
			this.repSwizChk2.Size = new System.Drawing.Size(120, 16);
			this.repSwizChk2.TabIndex = 16;
			this.repSwizChk2.Text = "Replicate Swizzles";
			// 
			// srcModChk2
			// 
			this.srcModChk2.Location = new System.Drawing.Point(240, 40);
			this.srcModChk2.Name = "srcModChk2";
			this.srcModChk2.Size = new System.Drawing.Size(120, 16);
			this.srcModChk2.TabIndex = 15;
			this.srcModChk2.Text = "Source Modifiers";
			// 
			// label7
			// 
			this.label7.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((System.Byte)(0)));
			this.label7.Location = new System.Drawing.Point(200, 16);
			this.label7.Name = "label7";
			this.label7.Size = new System.Drawing.Size(96, 16);
			this.label7.TabIndex = 14;
			this.label7.Text = "Allow:";
			// 
			// sourceCombo2
			// 
			this.sourceCombo2.Location = new System.Drawing.Point(16, 24);
			this.sourceCombo2.Name = "sourceCombo2";
			this.sourceCombo2.Size = new System.Drawing.Size(80, 64);
			this.sourceCombo2.TabIndex = 13;
			// 
			// srcGroup3
			// 
			this.srcGroup3.Controls.Add(this.initChk3);
			this.srcGroup3.Controls.Add(this.genSwizChk3);
			this.srcGroup3.Controls.Add(this.arbSwizChk3);
			this.srcGroup3.Controls.Add(this.repSwizChk3);
			this.srcGroup3.Controls.Add(this.srcModChk3);
			this.srcGroup3.Controls.Add(this.label8);
			this.srcGroup3.Controls.Add(this.sourceCombo3);
			this.srcGroup3.Enabled = false;
			this.srcGroup3.Location = new System.Drawing.Point(16, 496);
			this.srcGroup3.Name = "srcGroup3";
			this.srcGroup3.Size = new System.Drawing.Size(368, 100);
			this.srcGroup3.TabIndex = 20;
			this.srcGroup3.TabStop = false;
			this.srcGroup3.Text = "Source 3";
			// 
			// initChk3
			// 
			this.initChk3.Location = new System.Drawing.Point(240, 56);
			this.initChk3.Name = "initChk3";
			this.initChk3.Size = new System.Drawing.Size(96, 16);
			this.initChk3.TabIndex = 19;
			this.initChk3.Text = "Must Initialize";
			// 
			// genSwizChk3
			// 
			this.genSwizChk3.Location = new System.Drawing.Point(112, 32);
			this.genSwizChk3.Name = "genSwizChk3";
			this.genSwizChk3.Size = new System.Drawing.Size(112, 16);
			this.genSwizChk3.TabIndex = 18;
			this.genSwizChk3.Text = "General Swizzles";
			// 
			// arbSwizChk3
			// 
			this.arbSwizChk3.Location = new System.Drawing.Point(112, 64);
			this.arbSwizChk3.Name = "arbSwizChk3";
			this.arbSwizChk3.Size = new System.Drawing.Size(128, 16);
			this.arbSwizChk3.TabIndex = 17;
			this.arbSwizChk3.Text = "Arbitrary Swizzles";
			// 
			// repSwizChk3
			// 
			this.repSwizChk3.Location = new System.Drawing.Point(112, 48);
			this.repSwizChk3.Name = "repSwizChk3";
			this.repSwizChk3.Size = new System.Drawing.Size(120, 16);
			this.repSwizChk3.TabIndex = 16;
			this.repSwizChk3.Text = "Replicate Swizzles";
			// 
			// srcModChk3
			// 
			this.srcModChk3.Location = new System.Drawing.Point(240, 40);
			this.srcModChk3.Name = "srcModChk3";
			this.srcModChk3.Size = new System.Drawing.Size(120, 16);
			this.srcModChk3.TabIndex = 15;
			this.srcModChk3.Text = "Source Modifiers";
			// 
			// label8
			// 
			this.label8.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((System.Byte)(0)));
			this.label8.Location = new System.Drawing.Point(200, 16);
			this.label8.Name = "label8";
			this.label8.Size = new System.Drawing.Size(96, 16);
			this.label8.TabIndex = 14;
			this.label8.Text = "Allow:";
			// 
			// sourceCombo3
			// 
			this.sourceCombo3.Location = new System.Drawing.Point(16, 24);
			this.sourceCombo3.Name = "sourceCombo3";
			this.sourceCombo3.Size = new System.Drawing.Size(80, 64);
			this.sourceCombo3.TabIndex = 13;
			// 
			// statusBar
			// 
			this.statusBar.Location = new System.Drawing.Point(0, 681);
			this.statusBar.Name = "statusBar";
			this.statusBar.Size = new System.Drawing.Size(400, 22);
			this.statusBar.SizingGrip = false;
			this.statusBar.TabIndex = 21;
			// 
			// genFileBtn
			// 
			this.genFileBtn.Location = new System.Drawing.Point(264, 616);
			this.genFileBtn.Name = "genFileBtn";
			this.genFileBtn.Size = new System.Drawing.Size(120, 23);
			this.genFileBtn.TabIndex = 22;
			this.genFileBtn.Text = "Generate From File...";
			this.genFileBtn.Click += new System.EventHandler(this.genFileBtn_Click);
			// 
			// staticChk
			// 
			this.staticChk.Location = new System.Drawing.Point(232, 72);
			this.staticChk.Name = "staticChk";
			this.staticChk.Size = new System.Drawing.Size(128, 24);
			this.staticChk.TabIndex = 23;
			this.staticChk.Text = "Static Flow Control";
			// 
			// dynamicChk
			// 
			this.dynamicChk.Location = new System.Drawing.Point(232, 88);
			this.dynamicChk.Name = "dynamicChk";
			this.dynamicChk.Size = new System.Drawing.Size(136, 24);
			this.dynamicChk.TabIndex = 24;
			this.dynamicChk.Text = "Dynamic Flow Control";
			// 
			// allowedInDFCChk
			// 
			this.allowedInDFCChk.Location = new System.Drawing.Point(232, 104);
			this.allowedInDFCChk.Name = "allowedInDFCChk";
			this.allowedInDFCChk.Size = new System.Drawing.Size(112, 24);
			this.allowedInDFCChk.TabIndex = 25;
			this.allowedInDFCChk.Text = "Allowed In DFCs";
			// 
			// clearBtn
			// 
			this.clearBtn.Location = new System.Drawing.Point(232, 656);
			this.clearBtn.Name = "clearBtn";
			this.clearBtn.TabIndex = 26;
			this.clearBtn.Text = "Clear";
			this.clearBtn.Click += new System.EventHandler(this.clearBtn_Click);
			// 
			// SVCaseGenForm
			// 
			this.AutoScaleBaseSize = new System.Drawing.Size(5, 13);
			this.ClientSize = new System.Drawing.Size(400, 703);
			this.Controls.Add(this.clearBtn);
			this.Controls.Add(this.allowedInDFCChk);
			this.Controls.Add(this.dynamicChk);
			this.Controls.Add(this.staticChk);
			this.Controls.Add(this.genFileBtn);
			this.Controls.Add(this.statusBar);
			this.Controls.Add(this.srcGroup2);
			this.Controls.Add(this.srcGroup1);
			this.Controls.Add(this.label3);
			this.Controls.Add(this.destGroup);
			this.Controls.Add(this.sourceRegList);
			this.Controls.Add(this.label1);
			this.Controls.Add(this.Exit);
			this.Controls.Add(this.Generate);
			this.Controls.Add(this.label2);
			this.Controls.Add(this.instructionBox);
			this.Controls.Add(this.groupBox1);
			this.Controls.Add(this.srcGroup3);
			this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedSingle;
			this.Name = "SVCaseGenForm";
			this.Text = "Shader Validator Test Case Generator";
			this.groupBox1.ResumeLayout(false);
			this.destGroup.ResumeLayout(false);
			this.srcGroup1.ResumeLayout(false);
			this.srcGroup2.ResumeLayout(false);
			this.srcGroup3.ResumeLayout(false);
			this.ResumeLayout(false);

		}
		#endregion

		/// <summary>
		/// The main entry point for the application.
		/// </summary>
		[STAThread]
		static void Main() 
		{
			Application.Run(new SVCaseGenForm());
		}

		private void Generate_Click(object sender, System.EventArgs e)
		{
            if (instructionBox.Text == "" || instructionBox.Text.IndexOf(",", StringComparison.Ordinal) >= 0)
			{
				MessageBox.Show( "Please enter a valid shader instruction", "Error" );
				return;
			}

			GeneratorOptions options;
			options.ShaderVersion = (string) versionList.Items[ versionList.SelectedIndex ];
			options.Instruction = instructionBox.Text;
			options.StaticFlow = staticChk.Checked;
			options.DynamicFlow = dynamicChk.Checked;
			options.IsAllowedInDFC = allowedInDFCChk.Checked;
			options.NumSourceRegs = Int32.Parse( (string) sourceRegList.Items[ sourceRegList.SelectedIndex ] );
			options.WriteMasksAllowed = GetCheckedItems( writeMaskCombo );
			options.DestRegsAllowed = GetCheckedItems( destCombo );
			options.DestModsAllowed = GetCheckedItems( destModCombo );

			SrcRegOptions [ ] srcInfo = new SrcRegOptions [ options.NumSourceRegs ];
			for( int i = 0; i < srcInfo.Length; i++ )
			{
				srcInfo[ i ].SrcRegsAllowed = GetCheckedItems( sourceCombo[ i ] );
				srcInfo[ i ].SourceMod = srcModChk[ i ].Checked;
				srcInfo[ i ].ReplicateSwizzles = repSwizChk[ i ].Checked;
				srcInfo[ i ].ArbitrarySwizzles = arbSwizChk[ i ].Checked;
				srcInfo[ i ].GeneralSwizzles = genSwizChk[ i ].Checked;
				srcInfo[ i ].MustInitialize = initChk[ i ].Checked;
			}
			options.SrcRegInfo = srcInfo;

			GenerateCases( options, true );
		}

		private void GenerateCases( GeneratorOptions options, bool saveOptions )
		{
			statusBar.Text = "Generating test cases...";		
			string cases = caseGen.GenerateCases( options );

			if( saveOptions )
			{
				EditOutput edit = new EditOutput( );
				edit.SetText( cases );
				edit.ShowDialog( );
				DialogResult res = MessageBox.Show( "Save generator options?", "Save", MessageBoxButtons.YesNo );
				if( res == DialogResult.Yes )
					SaveOptions( options );
			}
			else
			{
				StreamWriter file = File.CreateText( OUTPUT_PATH + options.ShaderVersion + "_inst_" + options.Instruction + ".fx" );
				file.WriteLine( cases );
				file.Close( );
			}
			
			statusBar.Text = "";
		}

		private string [ ] GetCheckedItems( CheckedListBox list )
		{
			string [ ] ret = new string[ list.CheckedItems.Count ];
			list.CheckedItems.CopyTo( ret, 0 );
			return ret;
		}

		private void Exit_Click(object sender, System.EventArgs e)
		{
			this.Close( );
		}

		private void versionList_SelectedIndexChanged(object sender, System.EventArgs e)
		{
			ComboBox list = (ComboBox) sender;
			string version = (string) list.Items[ list.SelectedIndex ];
			string [ ] registers = caseGen.GetRegisters( version );
			string [ ] destMods = caseGen.GetDestMods( version );
			
			dynamicChk.Checked = true;
			staticChk.Checked = true;
			allowedInDFCChk.Checked = true;
			InitCheckList( writeMaskCombo, caseGen.GetWriteMasks( version ) );
			InitCheckList( destModCombo, destMods );
			InitCheckList( destCombo, registers );

			for( int i = 0; i < 3; i++ )
			{
				InitCheckList( sourceCombo[ i ], registers );
				srcModChk[ i ].Checked	= true;
				repSwizChk[ i ].Checked = true;
				arbSwizChk[ i ].Checked = true;
				genSwizChk[ i ].Checked = true;
				initChk[ i ].Checked	= true;
			}
		}

		private void InitCheckList( CheckedListBox list, string [ ] values )
		{
			list.Items.Clear( );
			if( values != null )
				list.Items.AddRange( values );
			for( int i = 0; i < list.Items.Count; i++ )
				list.SetItemChecked( i, true );
		}

		private void sourceRegList_SelectedIndexChanged(object sender, System.EventArgs e)
		{
			ComboBox list = (ComboBox) sender;
			int numRegs = Int32.Parse( (string) list.Items[ list.SelectedIndex ] );
			switch( numRegs )
			{
				case 0:
					srcGroup1.Enabled = false;
					srcGroup2.Enabled = false;
					srcGroup3.Enabled = false;
					break;
				case 1:
					srcGroup1.Enabled = true;
					srcGroup2.Enabled = false;
					srcGroup3.Enabled = false;
					break;
				case 2:
					srcGroup1.Enabled = true;
					srcGroup2.Enabled = true;
					srcGroup3.Enabled = false;
					break;
				case 3:
					srcGroup1.Enabled = true;
					srcGroup2.Enabled = true;
					srcGroup3.Enabled = true;
					break;
			}
		}

		private void genFileBtn_Click(object sender, System.EventArgs e)
		{
			OpenFileDialog fileDlg = new OpenFileDialog( );
			fileDlg.Filter = "Generator File (*.gen)|*.gen";
			string filename = null;
			if( fileDlg.ShowDialog( ) == DialogResult.OK )
				filename = fileDlg.FileName;
			else
				return;
			
			GeneratorOptions [ ] options = caseGen.ParseOptions( filename );
			try
			{
				foreach( GeneratorOptions o in options )
					GenerateCases( o, false );
			}
			catch( Exception ex )
			{
				MessageBox.Show( ex.Message, "Error" );
			}
		}

		private void SaveOptions( GeneratorOptions options )
		{
			SaveFileDialog fileDlg = new SaveFileDialog( );
			fileDlg.Filter = "Generator File (*.gen)|*.gen";
			fileDlg.OverwritePrompt = false;
			string filename = null;
			if( fileDlg.ShowDialog( ) == DialogResult.OK )
				filename = fileDlg.FileName;
			else
				return;

			try
			{
				caseGen.SaveOptions( options, filename );
			}
			catch( Exception e )
			{
				MessageBox.Show( e.Message, "Error" );
			}
		}

		private void clearBtn_Click(object sender, System.EventArgs e)
		{
			instructionBox.Text = "";
			versionList_SelectedIndexChanged( versionList, null );
			sourceRegList_SelectedIndexChanged( sourceRegList, null );
		}
	}
}
