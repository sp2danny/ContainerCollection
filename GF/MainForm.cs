using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace GF
{
    public partial class mainForm : Form
    {
        public mainForm()
        {
            InitializeComponent();
        }
 
        private void mainForm_Load(object sender, EventArgs e)
        {
            // load stuff
        }
 
        private void loadButton_Click(object sender, EventArgs e)
        {
            mainChart.Legends[0].Enabled = true;
            fittingTabControl.Enabled = true;

            mainChart.Series[0].Points.AddXY(12, 100);
            mainChart.Series[0].Points.AddXY(13, 120);
            mainChart.Series[0].Points.AddXY(17, 150);

            mainChart.Series[1].Points.AddXY(11, 110);
            mainChart.Series[1].Points.AddXY(13, 120);
            mainChart.Series[1].Points.AddXY(18, 130);

            mainChart.Series[2].Points.AddXY( 9,  80);
            mainChart.Series[2].Points.AddXY(13,  95);
            mainChart.Series[2].Points.AddXY(17, 110);
        }

        private void sourceComboBox_SelectedIndexChanged(object sender, EventArgs e)
        {
            loadButton.Enabled = true;
        }
    }
}
