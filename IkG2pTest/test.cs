using IKg2p;
using System.Reflection;
using System.Text;
using TinyPinyin;

namespace IKg2pTest
{
    class Program
    {
        static void Main(string[] args)
        {
            // 创建一个ZhG2p实例
            var zhG2p = new ZhG2p("mandarin");

            Assembly assembly = Assembly.GetExecutingAssembly();
            string resourceName = assembly.GetName().Name + "." + "op_lab.txt";

            int count = 0;
            int error = 0;

            Console.WriteLine(resourceName);
            using (Stream stream = assembly.GetManifestResourceStream(resourceName))
            {
                if (stream != null)
                {
                    using (StreamReader reader = new StreamReader(stream, Encoding.UTF8))
                    {
                        string content = reader.ReadToEnd();
                        string[] lines = content.Split(new[] { Environment.NewLine }, StringSplitOptions.None);

                        foreach (string line in lines)
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
                                    Console.WriteLine(key);
                                    Console.WriteLine(value);
                                    Console.WriteLine(result);
                                    Console.WriteLine();

                                    if(result.Split(" ").Length !=value.Split(" ").Length)
                                    {
                                        Console.WriteLine("Error.");
                                        Environment.Exit(0);
                                    }

                                    var resWords = result.Split(" ");
                                    for (int i = 0; i < wordSize; i++)
                                    {
                                        if (words[i] != resWords[i])
                                        {
                                            error++;
                                        }
                                    }

                                }
                            }
                        }
                    }
                }
                else
                {
                    Console.WriteLine("Resource not found.");
                }

            }

            double percentage = Math.Round(((double)error / (double)count) * 100.0, 2);
            Console.WriteLine(percentage);

            Console.WriteLine(error);
            Console.WriteLine(count);
        }
    }
}