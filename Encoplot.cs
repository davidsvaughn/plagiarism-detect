using System;
using System.Collections;
using System.Collections.Generic;
using System.Configuration.Assemblies;
using System.Linq;
using System.Text;
using System.IO;
using System.Threading.Tasks;
using System.Diagnostics;
using System.Runtime.InteropServices;

namespace plagiarism
{
    public class Encoplot
    {
        public static readonly int MAXLEN = 8000123;
        public static int K = 8;
        public static double T = 1;

        [DllImport("encoplot.dll")]
        public static extern int GetSize();

        [DllImport("encoplot.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern int encoplot_ff(int k, string fn1, string fn2, uint[] data, int length);

        [DllImport("encoplot.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern int encoplot_ss(int k, string txt1, string txt2, uint[] data, int length);

        public static void Test()
        {
            string s = "e";
            string fn1 = String.Format("../../../data/{0}1.txt", s);
            string fn2 = String.Format("../../../data/{0}2.txt", s);
            Encoplot.RunEncoplot(fn1, fn2);
        }

        static int enco_ff(string fn1, string fn2, uint[] data, int length)
        {
            return encoplot_ff(K, fn1, fn2, data, length);
        }

        static int enco_ss(string txt1, string txt2, uint[] data, int length)
        {
            return encoplot_ss(K, txt1, txt2, data, length);
        }

        public static int Encoplot_ss(string fn1, string fn2, out uint[] data)
        {
            data = new uint[MAXLEN];
            return enco_ss(File.ReadAllText(fn1), File.ReadAllText(fn2), data, MAXLEN);
        }

        public static int Encoplot_ff(string fn1, string fn2, out uint[] data)
        {
            data = new uint[MAXLEN];
            return enco_ff(fn1, fn2, data, MAXLEN);
        }

        public static void RunEncoplot(string fn1, string fn2)
        {
            // call c code
            uint[] data;
            int len = Encoplot_ss(fn1, fn2, out data);
            //int len = Encoplot_ff(fn1, fn2, out data);

            // copy to jagged array
            int n = len / 2;
            int[][] v = new int[n][];
            for (int i = 0; i < n; i++)
            {
                v[i] = new int[] { (int)data[2 * i], (int)data[2 * i + 1] };
                //Console.WriteLine("{0}\t{1}", w[i][0], w[i][1]);
            }

            // sort by 1st column
            Array.Sort(v, (a1, a2) =>
            {
                return (a1[0] - a2[0]) + ((a1[0] - a2[0]) == 0 ? (a1[1] - a2[1]) : 0);
            });

            // compress segments
            List<int[]> w = new List<int[]>();
            int j = 0;
            w.Add(new int[] { v[0][0], v[0][1], K, j });
            for (int i = 1; i < n; i++)
            {
                if ((v[i][0] == v[i - 1][0] + 1) && (v[i][1] == v[i - 1][1] + 1))
                {
                    w[j][2]++;
                }
                else
                {
                    w.Add(new int[] { v[i][0], v[i][1], K, ++j });
                }
            }

            // connect segments
            ConnectSegments(w, T);

            // print
            Console.WriteLine("");
            j = 0;
            foreach (int[] p in w)
            {
                Console.WriteLine("({0})\t{1}", j++, string.Join("\t", p));
            }

            //
            PrintSegments(w, fn1, fn2);
        }

        public static void ConnectSegments(List<int[]> w, double t = 1)
        {
            int n = w.Count;
            for (int i = 0; i < n; i++)
            {
                int j = 1, l1 = w[i][2], l2 = i + j < n ? w[i + j][2] : 0;
                int x = w[i][0] + l1, y = w[i][1] + l1;
                while (i + j < n && w[i + j][0] - x < t*Math.Max(l1, l2))
                {
                    //double h = t*Mean(l1, l2);
                    if (w[i + j][1] > y && w[i + j][1] - y < t*Math.Max(l1, l2))
                    {
                        w[i + j][3] = FindParent(w, i);
                        break;
                    }
                    if (i + (++j) < n)
                        l2 = w[i + j][2];
                }
            }
        }

        public static void PrintSegments(List<int[]> w, string fn1, string fn2)
        {
            List<int[]> v = new List<int[]>();
            Hashtable ht = new Hashtable();
            int n = w.Count;
            for (int i = 0; i < n; i++)
            {
                int g = w[i][3];
                if (ht[g] == null)
                {
                    ht[g] = ht.Count;
                    v.Add(new int[] {w[i][0], w[i][1], w[i][0] + w[i][2], w[i][1] + w[i][2]});
                }
                else
                {
                    int j = (int)ht[g];
                    v[j][2] = w[i][0] + w[i][2];
                    v[j][3] = w[i][1] + w[i][2];
                }
            }

            //
            int min = 50;
            string txt1 = File.ReadAllText(fn1), txt2 = File.ReadAllText(fn2);
            for (int i = 0; i < v.Count; i++)
            {
                if (Math.Min(v[i][2] - v[i][0], v[i][3] - v[i][1]) > min)
                {
                    Console.WriteLine("");
                    Console.WriteLine(txt1.Substring(v[i][0], v[i][2] - v[i][0]).Trim());
                    Console.WriteLine(txt2.Substring(v[i][1], v[i][3] - v[i][1]).Trim());
                }
            }
        }

        static double Mean(double a, double b)
        {
            return (a + b)/2d;
        }

        public static int FindParent(List<int[]> w, int i)
        {
            if (w[i][3] == i)
                return i;
            else
            {
                int p = FindParent(w, w[i][3]);
                w[i][3] = p;
                return p;
            }
        }
    }
}
