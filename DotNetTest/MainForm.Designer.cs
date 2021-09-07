namespace DotNetTest
{

    partial class MainForm
    {
        /// <summary>
        /// Required designer variable.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        /// Clean up any resources being used.
        /// </summary>
        /// <param name="disposing">true if managed resources should be disposed; otherwise, false.</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Windows Form Designer generated code

        /// <summary>
        /// Required method for Designer support - do not modify
        /// the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent()
        {
            this.splitContainer1 = new System.Windows.Forms.SplitContainer();
            this.drawSurface1 = new DotNetTest.DrawControl();
            this.drawSurface2 = new DotNetTest.DrawControl();
            ((System.ComponentModel.ISupportInitialize)(this.splitContainer1)).BeginInit();
            this.splitContainer1.Panel1.SuspendLayout();
            this.splitContainer1.Panel2.SuspendLayout();
            this.splitContainer1.SuspendLayout();
            this.SuspendLayout();
            // 
            // splitContainer1
            // 
            this.splitContainer1.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D;
            this.splitContainer1.Dock = System.Windows.Forms.DockStyle.Fill;
            this.splitContainer1.Location = new System.Drawing.Point(0, 0);
            this.splitContainer1.Name = "splitContainer1";
            // 
            // splitContainer1.Panel1
            // 
            this.splitContainer1.Panel1.Controls.Add(this.drawSurface1);
            // 
            // splitContainer1.Panel2
            // 
            this.splitContainer1.Panel2.Controls.Add(this.drawSurface2);
            this.splitContainer1.Size = new System.Drawing.Size(800, 450);
            this.splitContainer1.SplitterDistance = 399;
            this.splitContainer1.TabIndex = 2;
            // 
            // drawSurface1
            // 
            this.drawSurface1.Dock = System.Windows.Forms.DockStyle.Fill;
            this.drawSurface1.Location = new System.Drawing.Point(0, 0);
            this.drawSurface1.Name = "drawSurface1";
            this.drawSurface1.Size = new System.Drawing.Size(395, 446);
            this.drawSurface1.TabIndex = 0;
            this.drawSurface1.Paint += new System.Windows.Forms.PaintEventHandler(this.DrawSurface1_Paint);
            // 
            // drawSurface2
            // 
            this.drawSurface2.Dock = System.Windows.Forms.DockStyle.Fill;
            this.drawSurface2.Location = new System.Drawing.Point(0, 0);
            this.drawSurface2.Name = "drawSurface2";
            this.drawSurface2.Size = new System.Drawing.Size(393, 446);
            this.drawSurface2.TabIndex = 1;
            this.drawSurface2.Paint += new System.Windows.Forms.PaintEventHandler(this.DrawSurface2_Paint);
            // 
            // MainForm
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(800, 450);
            this.Controls.Add(this.splitContainer1);
            this.Name = "MainForm";
            this.Text = "Test Form";
            this.Load += new System.EventHandler(this.MainForm_Load);
            this.splitContainer1.Panel1.ResumeLayout(false);
            this.splitContainer1.Panel2.ResumeLayout(false);
            ((System.ComponentModel.ISupportInitialize)(this.splitContainer1)).EndInit();
            this.splitContainer1.ResumeLayout(false);
            this.ResumeLayout(false);

        }

        #endregion

        private DrawControl drawSurface1;
        private DrawControl drawSurface2;
        private System.Windows.Forms.SplitContainer splitContainer1;
    }
}

