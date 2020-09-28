using System;
using System.Drawing;
using System.Collections;
using System.IO;
using System.ComponentModel;
using System.Windows.Forms;

namespace SVCaseGen
{
	/// <summary>
	/// Summary description for EditOutput.
	/// </summary>
	public class EditOutput : System.Windows.Forms.Form
	{
		private System.Windows.Forms.TextBox editBox;
		private System.Windows.Forms.Button saveBtn;
		private System.Windows.Forms.Button closeBtn;
		/// <summary>
		/// Required designer variable.
		/// </summary>
		private System.ComponentModel.Container components = null;

		public EditOutput()
		{
			//
			// Required for Windows Form Designer support
			//
			InitializeComponent();

			//
			// TODO: Add any constructor code after InitializeComponent call
			//
		}

		/// <summary>
		/// Clean up any resources being used.
		/// </summary>
		protected override void Dispose( bool disposing )
		{
			if( disposing )
			{
				if(components != null)
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
			this.editBox = new System.Windows.Forms.TextBox();
			this.saveBtn = new System.Windows.Forms.Button();
			this.closeBtn = new System.Windows.Forms.Button();
			this.SuspendLayout();
			// 
			// editBox
			// 
			this.editBox.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom) 
				| System.Windows.Forms.AnchorStyles.Left) 
				| System.Windows.Forms.AnchorStyles.Right)));
			this.editBox.Font = new System.Drawing.Font("Courier New", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((System.Byte)(0)));
			this.editBox.Location = new System.Drawing.Point(8, 8);
			this.editBox.Multiline = true;
			this.editBox.Name = "editBox";
			this.editBox.ScrollBars = System.Windows.Forms.ScrollBars.Both;
			this.editBox.Size = new System.Drawing.Size(552, 528);
			this.editBox.TabIndex = 0;
			this.editBox.Text = "";
			// 
			// saveBtn
			// 
			this.saveBtn.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
			this.saveBtn.Location = new System.Drawing.Point(408, 544);
			this.saveBtn.Name = "saveBtn";
			this.saveBtn.TabIndex = 1;
			this.saveBtn.Text = "Save...";
			this.saveBtn.Click += new System.EventHandler(this.saveBtn_Click);
			// 
			// closeBtn
			// 
			this.closeBtn.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
			this.closeBtn.Location = new System.Drawing.Point(488, 544);
			this.closeBtn.Name = "closeBtn";
			this.closeBtn.TabIndex = 2;
			this.closeBtn.Text = "Close";
			this.closeBtn.Click += new System.EventHandler(this.closeBtn_Click);
			// 
			// EditOutput
			// 
			this.AutoScaleBaseSize = new System.Drawing.Size(5, 13);
			this.ClientSize = new System.Drawing.Size(568, 573);
			this.Controls.Add(this.closeBtn);
			this.Controls.Add(this.saveBtn);
			this.Controls.Add(this.editBox);
			this.Name = "EditOutput";
			this.Text = "Generated Cases";
			this.ResumeLayout(false);

		}
		#endregion

		public void SetText( string text )
		{
			editBox.Text = text;
		}

		private void saveBtn_Click(object sender, System.EventArgs e)
		{
			FileDialog fileDlg = new SaveFileDialog( );
			fileDlg.Filter = "FX files (*.fx)|*.fx|All files (*.*)|*.*";
			string filename = null;
			switch( fileDlg.ShowDialog( ) )
			{
				case DialogResult.OK:
					filename = fileDlg.FileName;
					break;
			}
			if( filename != null )
				SaveToFile( filename );
		}

		private void SaveToFile( string filename )
		{
			StreamWriter file = File.CreateText( filename );
			try 
			{
				file.WriteLine( editBox.Text );
			}
			catch( Exception e ) 
			{ 
				Console.Error.WriteLine( e.StackTrace );
			}
			finally
			{
				file.Close( );
			}
			this.Close( );
		}

		private void closeBtn_Click(object sender, System.EventArgs e)
		{
			this.Close( );
		}
	}
}
