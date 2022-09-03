using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace WindowsFormsApp1
{
    public partial class Form1 : Form
    {
        public Form1()
        {
            InitializeComponent();
            serialPort1.BaudRate = 115200;
            serialPort1.PortName = "COM6";

            serialPort1.Open();
            if (serialPort1.IsOpen == true)
            {
                MessageBox.Show("Açıldı");
            }
            else 
            {
                MessageBox.Show("Açılamadı");

            }

        }

        private void button1_Click(object sender, EventArgs e)
        {
            string gonderilecekMsg;
            gonderilecekMsg = textBox1.Text;

            serialPort1.WriteLine(gonderilecekMsg);
        }

        private void serialPort1_DataReceived(object sender, System.IO.Ports.SerialDataReceivedEventArgs e)
        {
            string data = serialPort1.ReadExisting();
            if (richTextBox1.InvokeRequired)
            {
                richTextBox1.Invoke(new MethodInvoker(delegate { richTextBox1.Text += data + "\r\n"; }));
            }
        }
    }
}