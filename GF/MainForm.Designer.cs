namespace GF
{
    partial class mainForm
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
            System.Windows.Forms.DataVisualization.Charting.ChartArea chartArea1 = new System.Windows.Forms.DataVisualization.Charting.ChartArea();
            System.Windows.Forms.DataVisualization.Charting.Legend legend1 = new System.Windows.Forms.DataVisualization.Charting.Legend();
            System.Windows.Forms.DataVisualization.Charting.Series series1 = new System.Windows.Forms.DataVisualization.Charting.Series();
            System.Windows.Forms.DataVisualization.Charting.Series series2 = new System.Windows.Forms.DataVisualization.Charting.Series();
            System.Windows.Forms.DataVisualization.Charting.Series series3 = new System.Windows.Forms.DataVisualization.Charting.Series();
            this.mainChart = new System.Windows.Forms.DataVisualization.Charting.Chart();
            this.loadButton = new System.Windows.Forms.Button();
            this.fittingTabControl = new System.Windows.Forms.TabControl();
            this.insertTabPage = new System.Windows.Forms.TabPage();
            this.spliceTabPage = new System.Windows.Forms.TabPage();
            this.sortTabPage = new System.Windows.Forms.TabPage();
            this.sourceComboBox = new System.Windows.Forms.ComboBox();
            ((System.ComponentModel.ISupportInitialize)(this.mainChart)).BeginInit();
            this.fittingTabControl.SuspendLayout();
            this.SuspendLayout();
            // 
            // mainChart
            // 
            this.mainChart.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom) 
            | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            chartArea1.Name = "ChartArea1";
            this.mainChart.ChartAreas.Add(chartArea1);
            this.mainChart.ImeMode = System.Windows.Forms.ImeMode.Disable;
            legend1.Enabled = false;
            legend1.LegendStyle = System.Windows.Forms.DataVisualization.Charting.LegendStyle.Column;
            legend1.Name = "Legend1";
            this.mainChart.Legends.Add(legend1);
            this.mainChart.Location = new System.Drawing.Point(22, 27);
            this.mainChart.Name = "mainChart";
            series1.ChartArea = "ChartArea1";
            series1.ChartType = System.Windows.Forms.DataVisualization.Charting.SeriesChartType.Point;
            series1.Legend = "Legend1";
            series1.LegendText = "Insert";
            series1.Name = "Series1";
            series1.XValueType = System.Windows.Forms.DataVisualization.Charting.ChartValueType.Double;
            series1.YValueType = System.Windows.Forms.DataVisualization.Charting.ChartValueType.Double;
            series2.ChartArea = "ChartArea1";
            series2.ChartType = System.Windows.Forms.DataVisualization.Charting.SeriesChartType.Point;
            series2.Legend = "Legend1";
            series2.LegendText = "Splice";
            series2.Name = "Series2";
            series2.XValueType = System.Windows.Forms.DataVisualization.Charting.ChartValueType.Double;
            series2.YValueType = System.Windows.Forms.DataVisualization.Charting.ChartValueType.Double;
            series3.ChartArea = "ChartArea1";
            series3.ChartType = System.Windows.Forms.DataVisualization.Charting.SeriesChartType.Point;
            series3.Legend = "Legend1";
            series3.LegendText = "Sort";
            series3.Name = "Series3";
            series3.XValueType = System.Windows.Forms.DataVisualization.Charting.ChartValueType.Double;
            series3.YValueType = System.Windows.Forms.DataVisualization.Charting.ChartValueType.Double;
            this.mainChart.Series.Add(series1);
            this.mainChart.Series.Add(series2);
            this.mainChart.Series.Add(series3);
            this.mainChart.Size = new System.Drawing.Size(575, 498);
            this.mainChart.TabIndex = 1;
            this.mainChart.TabStop = false;
            this.mainChart.Text = "data fitting";
            // 
            // loadButton
            // 
            this.loadButton.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left)));
            this.loadButton.Enabled = false;
            this.loadButton.Location = new System.Drawing.Point(171, 531);
            this.loadButton.Name = "loadButton";
            this.loadButton.Size = new System.Drawing.Size(75, 23);
            this.loadButton.TabIndex = 2;
            this.loadButton.Text = "Load";
            this.loadButton.UseVisualStyleBackColor = true;
            this.loadButton.Click += new System.EventHandler(this.loadButton_Click);
            // 
            // fittingTabControl
            // 
            this.fittingTabControl.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.fittingTabControl.Controls.Add(this.insertTabPage);
            this.fittingTabControl.Controls.Add(this.spliceTabPage);
            this.fittingTabControl.Controls.Add(this.sortTabPage);
            this.fittingTabControl.Enabled = false;
            this.fittingTabControl.Location = new System.Drawing.Point(603, 27);
            this.fittingTabControl.Name = "fittingTabControl";
            this.fittingTabControl.SelectedIndex = 0;
            this.fittingTabControl.Size = new System.Drawing.Size(266, 498);
            this.fittingTabControl.TabIndex = 3;
            // 
            // insertTabPage
            // 
            this.insertTabPage.Location = new System.Drawing.Point(4, 22);
            this.insertTabPage.Name = "insertTabPage";
            this.insertTabPage.Padding = new System.Windows.Forms.Padding(3);
            this.insertTabPage.Size = new System.Drawing.Size(258, 472);
            this.insertTabPage.TabIndex = 0;
            this.insertTabPage.Text = "insert";
            this.insertTabPage.UseVisualStyleBackColor = true;
            // 
            // spliceTabPage
            // 
            this.spliceTabPage.Location = new System.Drawing.Point(4, 22);
            this.spliceTabPage.Name = "spliceTabPage";
            this.spliceTabPage.Padding = new System.Windows.Forms.Padding(3);
            this.spliceTabPage.Size = new System.Drawing.Size(258, 472);
            this.spliceTabPage.TabIndex = 1;
            this.spliceTabPage.Text = "splice";
            this.spliceTabPage.UseVisualStyleBackColor = true;
            // 
            // sortTabPage
            // 
            this.sortTabPage.Location = new System.Drawing.Point(4, 22);
            this.sortTabPage.Name = "sortTabPage";
            this.sortTabPage.Size = new System.Drawing.Size(258, 472);
            this.sortTabPage.TabIndex = 2;
            this.sortTabPage.Text = "sort";
            this.sortTabPage.UseVisualStyleBackColor = true;
            // 
            // sourceComboBox
            // 
            this.sourceComboBox.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left)));
            this.sourceComboBox.DisplayMember = "1";
            this.sourceComboBox.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.sourceComboBox.FormattingEnabled = true;
            this.sourceComboBox.Items.AddRange(new object[] {
            "std::vector",
            "avl_tree",
            "std::list"});
            this.sourceComboBox.Location = new System.Drawing.Point(22, 532);
            this.sourceComboBox.Name = "sourceComboBox";
            this.sourceComboBox.Size = new System.Drawing.Size(121, 21);
            this.sourceComboBox.TabIndex = 1;
            this.sourceComboBox.SelectedIndexChanged += new System.EventHandler(this.sourceComboBox_SelectedIndexChanged);
            // 
            // mainForm
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(881, 566);
            this.Controls.Add(this.sourceComboBox);
            this.Controls.Add(this.fittingTabControl);
            this.Controls.Add(this.loadButton);
            this.Controls.Add(this.mainChart);
            this.Name = "mainForm";
            this.Text = "GF";
            this.Load += new System.EventHandler(this.mainForm_Load);
            ((System.ComponentModel.ISupportInitialize)(this.mainChart)).EndInit();
            this.fittingTabControl.ResumeLayout(false);
            this.ResumeLayout(false);

        }

        #endregion
        private System.Windows.Forms.DataVisualization.Charting.Chart mainChart;
        private System.Windows.Forms.Button loadButton;
        private System.Windows.Forms.TabControl fittingTabControl;
        private System.Windows.Forms.TabPage insertTabPage;
        private System.Windows.Forms.TabPage spliceTabPage;
        private System.Windows.Forms.TabPage sortTabPage;
        private System.Windows.Forms.ComboBox sourceComboBox;
    }
}

