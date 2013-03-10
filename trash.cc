// Makefile: -lavfilter

  avfilter_register_all();

#include <libavfilter/avfilter.h>

  AVFilterContext* buffersink_ctx_;
  AVFilterContext* buffersrc_ctx_;
  AVFilterGraph* filter_graph_;

  InitFilters("scale=78:24");

int Movie::InitFilters(const string& filters_descr) {
  char args[512];
  int ret;
  AVFilter* buffersrc = avfilter_get_by_name("buffer");
  AVFilter* buffersink = avfilter_get_by_name("ffbuffersink");
  AVFilterInOut* outputs = avfilter_inout_alloc();
  AVFilterInOut* inputs = avfilter_inout_alloc();
  AVPixelFormat pix_fmts[] = {AV_PIX_FMT_GRAY8, AV_PIX_FMT_NONE};
  AVBufferSinkParams* buffersink_params;
  filter_graph = avfilter_graph_alloc();
  snprintf(args, sizeof(args),
           "video_size=%dx%d:pix_fmt=%d:time_base=%d/%d:pixel_aspect=%d/%d",
           dec_ctx->width, dec_ctx->height, dec_ctx->pix_fmt,
           dec_ctx->time_base.num, dec_ctx->time_base.den,
           dec_ctx->sample_aspect_ratio.num, dec_ctx->sample_aspect_ratio.den);
  ret = avfilter_graph_create_filter(&buffersrc_ctx, buffersrc, "in",
                                     args, NULL, filter_graph);
  if (ret < 0) {
    av_log(NULL, AV_LOG_ERROR, "Cannot create buffer source\n");
    return ret;
  }
  /* buffer video sink: to terminate the filter chain. */
  buffersink_params = av_buffersink_params_alloc();
  buffersink_params->pixel_fmts = pix_fmts;
  ret = avfilter_graph_create_filter(&buffersink_ctx, buffersink, "out",
                                     NULL, buffersink_params, filter_graph);
  av_free(buffersink_params);
  if (ret < 0) {
    av_log(NULL, AV_LOG_ERROR, "Cannot create buffer sink\n");
    return ret;
  }
  /* Endpoints for the filter graph. */
  outputs->name       = av_strdup("in");
  outputs->filter_ctx = buffersrc_ctx;
  outputs->pad_idx    = 0;
  outputs->next       = NULL;
  inputs->name       = av_strdup("out");
  inputs->filter_ctx = buffersink_ctx;
  inputs->pad_idx    = 0;
  inputs->next       = NULL;
  if ((ret = avfilter_graph_parse(filter_graph, filters_descr,
                                  &inputs, &outputs, NULL)) < 0)
    return ret;
  if ((ret = avfilter_graph_config(filter_graph, NULL)) < 0)
    return ret;
  return 0;
}

//////////////////////////////////////////////////////////////////////
// xterm hack #1

struct Combo {
  Combo(Pixel color, wchar_t ch, uint8_t xterm_fg, uint8_t xterm_bg)
      : color(color), ch(ch), xterm_fg(xterm_fg), xterm_bg(xterm_bg) {}
  Pixel color;
  wchar_t ch;
  int xterm_fg;
  int xterm_bg;
};

// std::vector<int> g_combos_red;
// std::vector<int> g_combos_green;
// std::vector<int> g_combos_blue;
// std::unordered_map<int, Combo> g_combos;
std::vector<Combo> g_combos;

void InitXterm256Hack1() {
  for (int xterm_bg = 17; xterm_bg < 256; ++xterm_bg) {
    g_combos.emplace_back(xterm_to_rgb(xterm_bg), L' ', 0, xterm_bg);
  }
  for (int xterm_fg = 17; xterm_fg < 232; ++xterm_fg) {
    for (int xterm_bg = 17; xterm_bg < 232; ++xterm_bg) {
      Pixel bg = xterm_to_rgb(xterm_bg);
      Pixel fg = xterm_to_rgb(xterm_fg);
      bg.MixKubelkaMunk(fg);
      g_combos.emplace_back(bg, L'\u2591', xterm_fg, xterm_bg);
    }
  }
}

const Combo& QuantizeXterm256Hack1(const Pixel& color) {
  const Combo* best = nullptr;
  double best_dist = 1e6;
  for (const auto& combo : g_combos) {
    double dist = color.Distance(combo.color);
    if (dist < best_dist) {
      best = &combo;
      best_dist = dist;
    }
  }
  CHECK_NOTNULL(best);
  assert(best != nullptr);
  return *best;
}

void PrintImageXterm256Hack1(std::ostream& os, const Graphic& graphic) {
  TermPrinter out(os);
  Pixel bg = Pixel(FLAGS_bg);
  int bg256 = rgb_to_xterm256(bg);
  for (int y = 0; y < graphic.height(); ++y) {
    for (int x = 0; x < graphic.width(); ++x) {
      const Pixel& color = graphic.Get(x, y).Copy().Opacify(bg);
      const Combo& combo = QuantizeXterm256Hack1(color);
      if (combo.xterm_fg)
        out.SetForeground256(combo.xterm_fg);
      if (combo.xterm_bg)
        out.SetBackground256(combo.xterm_bg);
      if (!FLAGS_bgprint && (combo.xterm_fg == bg256 &&
                             combo.xterm_bg == bg256)) {
        out << FLAGS_space;
      } else {
        out << combo.ch;
      }
    }
    out.Reset();
    out << "\n";
  }
}

  if (FLAGS_xterm256_hack1)
    InitXterm256Hack1();

    if (FLAGS_xterm256_hack1) {
      PrintImageXterm256Hack1(
          os, graphic.BilinearScale(width, height / 2));
    }

// hack #1
//////////////////////////////////////////////////////////////////////

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
