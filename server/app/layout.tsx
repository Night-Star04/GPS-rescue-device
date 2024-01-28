import type { ReactNode } from "react";
import type { Metadata } from "next";
import { Roboto } from "next/font/google";
const roboto = Roboto({ subsets: ["latin"], weight: ["400", "700"] });

const metadata: Metadata = {
  title: "GPS Rescue Device System",
  description: "GPS Rescue Device System",
};

function RootLayout({ children }: { children: ReactNode }) {
  return (
    <html lang="zh-Hans-TW">
      <body className={roboto.className}>{children}</body>
    </html>
  );
}

export default RootLayout;
export { metadata };
