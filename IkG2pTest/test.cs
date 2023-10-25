using IKg2p;
using System.Diagnostics;
using System.Reflection;
using System.Text;
using TinyPinyin;
using EzPinyin;

namespace IKg2pTest
{
    class Program
    {
        static string[] ReadData(string sourceName)
        {
            Assembly assembly = Assembly.GetExecutingAssembly();
            string resourceName = assembly.GetName().Name + "." + sourceName;
            using (Stream stream = assembly.GetManifestResourceStream(resourceName))
            {
                if (stream != null)
                {
                    using (StreamReader reader = new StreamReader(stream, Encoding.UTF8))
                    {
                        string content = reader.ReadToEnd();
                        return content.Split(new[] { Environment.NewLine }, StringSplitOptions.None);
                    }
                }
                else
                {
                    Console.WriteLine("Resource not found.");
                    return new string[0];
                }
            }
        }
        static void Main(string[] args)
        {
            string[] dataLines = ReadData("op_lab.txt");
            Stopwatch stopwatch = new Stopwatch();
            stopwatch.Start();

            // 创建一个ZhG2p实例
            var zhG2p = new ZhG2p("mandarin");

            StreamWriter writer = new StreamWriter("out.txt");
            int count = 0;
            int error = 0;
            if (dataLines.Length > 0)
            {
                foreach (string line in dataLines)
                {
                    string trimmedLine = line.Trim();
                    string[] keyValuePair = trimmedLine.Split('|');


                    if (keyValuePair.Length == 2)
                    {
                        string key = keyValuePair[0];
                        string value = keyValuePair[1];
                        string result = zhG2p.Convert(key, false, true);
                        // var result = TinyPinyin.PinyinHelper.GetPinyin(key).ToLower();

                        var words = value.Split(" ");
                        int wordSize = words.Length;
                        count += wordSize;

                        if (result != value)
                        {
                            Console.WriteLine("text: " + key);
                            Console.WriteLine("raw: " + value);
                            Console.Write("out:");
                            writer.WriteLine(trimmedLine);

                            var resWords = result.Split(" ");
                            for (int i = 0; i < wordSize; i++)
                            {
                                if (words[i] != resWords[i])
                                {
                                    Console.ForegroundColor = ConsoleColor.Red;
                                    Console.Write(" " + resWords[i]);
                                    error++;
                                }
                                else
                                {
                                    Console.ResetColor();
                                    Console.Write(" " + resWords[i]);
                                }
                            }
                            Console.ResetColor();
                            Console.WriteLine();
                            Console.WriteLine();
                        }
                    }
                }
                writer.Close();
                stopwatch.Stop();

                double percentage = Math.Round(((double)error / (double)count) * 100.0, 2);
                Console.WriteLine("错误率: " + percentage + "%");
                Console.WriteLine("错误数: " + error);
                Console.WriteLine("总字数: " + count);

                TimeSpan elapsedTime = stopwatch.Elapsed;
                Console.WriteLine("函数执行时间: " + elapsedTime.TotalMilliseconds + " 毫秒");
            }
        }
    }
}