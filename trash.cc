
void HideCursor() {
  g_cursor_saved = true;
  cout << "\x1b[?25l\x1b[s";
}

void ShowCursor() {
  g_cursor_saved = false;
  cout << "\x1b[u\x1b[?25h";
}

void ResetCursor() {
  cout << "\x1b[H";
}


  signal(SIGINT, OnCtrlC);
  struct stat dinfo;
  stat(path.data(), &dinfo);
  if (S_ISDIR(dinfo.st_mode)) {
    // If directory, print a movie using all the frames..
    cout << "Printing a Movie from directory.\n";
    PrintMovie(path, 1000);
    exit(0);
  }

// Prints all the frames from a directory.
// Assumes mplayer generated all these from a .jpg
void PrintMovie(const string& dir, const int frames) {
  HideCursor();
  for (int frame = 1; frame <= frames; ++frame) {
    ResetCursor();
    std::stringstream ss;
    char buf[128];
    snprintf(buf, sizeof(buf), "%s/%08d.jpg", dir.data(), frame);
    PrintImage(ss, LoadJPEG(buf));
    cout << ss.str();
    if (FLAGS_stepthrough) {
      string lol;
      std::getline(std::cin, lol);
    }
  }
  ShowCursor();
}


  // Graphic lol(256, 3);
  // for (int fg = 17; fg < 232; ++fg) {
  //   Pixel pix = xterm_to_rgb(fg);
  //   lol.Get(pix.red() * 255, 0) = Pixel::kBlack;
  //   lol.Get(pix.green() * 255, 1) = Pixel::kBlack;
  //   lol.Get(pix.blue() * 255, 2) = Pixel::kBlack;
  // }
  // WritePNG(lol, "/home/jart/www/graphic.png");

  // Graphic lol(256, 256, Pixel::Parse("grey"));
  // for (int fg = 16; fg < 256; ++fg) {
  //   Pixel pix = xterm_to_rgb(fg);
  //   cout << fg << " = " << pix << "\n";
  //   lol.Get(pix.red() * 255,
  //           pix.blue() * 255) = Pixel(0, 0, pix.blue());
  // }
  // WritePNG(lol, "/home/jart/www/graphic.png");

  // InitXterm256Hack1();
  // Graphic lol(256, 256, Pixel::Parse("grey"));
  // int n = 0;
  // for (const auto& combo : g_combos) {
  //   n++;
  //   Pixel pix = combo.color;
  //   CHECK(pix.red() >= 0) << n;
  //   lol.Get(pix.red() * 255,
  //           pix.green() * 255) = Pixel(0, 0, pix.blue());
  // }
  // WritePNG(lol, "/home/jart/www/graphic.png");

  // TermPrinter out(cout, Pixel::Parse(FLAGS_bg), FLAGS_bgprint);
  // out.SetBold(true);
  // out << "hello\n";
  PrintImage(cout, LoadPNG("balls.png"));
  PrintImage(cout, LoadJPEG("obama.jpg"));
  // PrintImage(cout, LoadLetter(L'@', Pixel::kWhite, Pixel::kClear));

  // InitXterm256Hack1();
  // Graphic spectrum = GenerateSpectrum(200, 100);
  // for (int y = 0; y < spectrum.height(); ++y) {
  //   for (int x = 0; x < spectrum.width(); ++x) {
  //     spectrum.Get(x, y) = QuantizeXterm256Hack1(spectrum.Get(x, y)).color;
  //     spectrum.Get(x, y) = xterm_to_rgb(rgb_to_xterm256(spectrum.Get(x, y)));
  //   }
  // }
  // WritePNG(spectrum, "/home/jart/www/graphic.png");

  // TermPrinter out(cout, Pixel::kBlack, false);
  // out.SetBackground256(Pixel::kGreen);
  // out.SetForeground256(Pixel::kGreen);
  // out << L'\u2580';
  // out << L'\u2580';
  // out << L'\u2580';
  // out << L'\u2580';
  // out << L'\n';

  // cout << "\x1b[?25l";  // Hide cursor.
  // for (int frame = 1; frame <= 1000; ++frame) {
  //   std::stringstream ss;
  //   ss << "\x1b[H\n";
  //   char buf[128];
  //   snprintf(buf, sizeof(buf), "rickroll/%08d.jpg", frame);
  //   PrintImage(ss, LoadJPEG(buf));
  //   cout << ss.str();
  //   // timespec req = {0, 50000000};
  //   // nanosleep(&req, NULL);
  // }

  // for (int code = 40; code < 256; ++code) {
  //   std::ostringstream out;
  //   string val;
  //   Pixel pix;

  //   for (int n = 0; n < 10; ++n) {
  //     cout << "\x1b[38;5;" << code << "m"
  //          << wstring(80, kFullBlock)
  //          << "\x1b[0m\n";
  //   }
  //   cout << "What dost thou see? ";
  //   do { std::cin >> val; } while (val == "");
  //   pix = Pixel(val);
  //   out << "[0][" << code << "] = {"
  //       << static_cast<int>(pix.red() * 255) << ", "
  //       << static_cast<int>(pix.green() * 255) << ", "
  //       << static_cast<int>(pix.blue() * 255) << "},"
  //       << "\n";
  //   cout << "\n";

  //   for (int n = 0; n < 10; ++n) {
  //     cout << "\x1b[48;5;" << code << "m"
  //          << string(80, L' ')
  //          << "\x1b[0m\n";
  //   }
  //   cout << "What dost thou see? ";
  //   do { std::cin >> val; } while (val == "");
  //   pix = Pixel(val);
  //   out << "[1][" << code << "] = {"
  //       << static_cast<int>(pix.red() * 255) << ", "
  //       << static_cast<int>(pix.green() * 255) << ", "
  //       << static_cast<int>(pix.blue() * 255) << "},"
  //       << "\n";
  //   cout << "\n";

  //   std::ofstream("terminal.app.txt", std::ios_base::app) << out.str();
  // }
