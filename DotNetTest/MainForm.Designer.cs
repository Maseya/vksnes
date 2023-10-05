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
            this.drawSurface = new SnesRenderNet.DrawControl();
            this.SuspendLayout();
            // 
            // drawSurface1
            // 
            this.drawSurface.Dock = System.Windows.Forms.DockStyle.Fill;
            this.drawSurface.Location = new System.Drawing.Point(0, 0);
            this.drawSurface.Name = "drawSurface1";
            this.drawSurface.Size = new System.Drawing.Size(514, 514);
            this.drawSurface.TabIndex = 0;
            this.drawSurface.Paint += new System.Windows.Forms.PaintEventHandler(this.DrawSurface1_Paint);
            // 
            // MainForm
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(514, 514);
            this.Controls.Add(this.drawSurface);
            this.Name = "MainForm";
            this.Text = "Test Form";
            this.Load += new System.EventHandler(this.MainForm_Load);
            this.ResumeLayout(false);

        }

        #endregion

        private SnesRenderNet.DrawControl drawSurface;
    }
}

